#include <windows.h>
#include <aclapi.h>
#include <cstdio>
void PrintLastError(const char *msg)
{
  DWORD errorMessageID = GetLastError();
  if (errorMessageID != 0)
  {
    LPSTR messageBuffer = NULL;

    // Получаем строку сообщения об ошибке
    DWORD size = FormatMessageA(
      FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
      NULL, errorMessageID, 0, (LPSTR) &messageBuffer, 0, NULL);
    if (size)
    {
      // Выводим сообщение об ошибке с пояснением
      printf("%s: %s\n", msg, messageBuffer);
    }
    else
    {
      printf("%s: Unknown error (error code: %lu)\n", msg, errorMessageID);
    }

    // Освобождаем буфер сообщения
    if (messageBuffer)
    {
      LocalFree(messageBuffer);
    }
  }
  else
  {
    printf("%s: The error code is missing.\n", msg);
  }
}
int main()
{
  LPCWSTR fileName = L"./example.txt";

  // Шаг 1: Получение SID текущего пользователя
  HANDLE hToken = NULL;
  OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken);
  DWORD dwBufferSize = 0;
  GetTokenInformation(hToken, TokenUser, NULL, 0, &dwBufferSize);
  PTOKEN_USER pTokenUser = (PTOKEN_USER) malloc(dwBufferSize);
  if (!GetTokenInformation(hToken, TokenUser, pTokenUser, dwBufferSize, &dwBufferSize))
  {
    PrintLastError("Error retrieving user information");
    return 1;
  }
  PSID pSid = pTokenUser->User.Sid;

  // Шаг 2: Получение текущего дескриптора безопасности
  PSECURITY_DESCRIPTOR pSD = NULL;
  PACL pOldAcl = NULL;
  if (GetNamedSecurityInfo((LPSTR) fileName,
                           SE_FILE_OBJECT,
                           DACL_SECURITY_INFORMATION,
                           NULL, NULL, &pOldAcl, NULL,
                           &pSD) != ERROR_SUCCESS)
  {
    PrintLastError("Error receiving file security information");
    return 1;
  }

  // Шаг 3: Получение прав доступа с помощью GetEffectiveRightsFromAcl
  DWORD dwAccessRights = 0;
  TRUSTEE trustee;
  BuildTrusteeWithSid(&trustee, pSid);
  if (GetEffectiveRightsFromAcl(pOldAcl, &trustee, &dwAccessRights) != ERROR_SUCCESS)
  {
    PrintLastError("Error checking effective access rights");
    return 1;
  }
  printf("Effective access rights: 0x%x\n", dwAccessRights);

  // Шаг 4: Изменение ACL, добавление ACE
  PACL pNewAcl = NULL;
  DWORD dwAclSize = sizeof(ACL) + 2 * (sizeof(ACCESS_ALLOWED_ACE) + GetLengthSid(pSid) - sizeof(DWORD));
  pNewAcl = (PACL) LocalAlloc(LPTR, dwAclSize);
  if (!InitializeAcl(pNewAcl, dwAclSize, ACL_REVISION))
  {
    PrintLastError("Error initializing the ACL");
    return 1;
  }

  // Добавляем запрещающие ACE
  if (!AddAccessDeniedAce(pNewAcl, ACL_REVISION, GENERIC_WRITE, pSid))
  {
    PrintLastError("Error when adding a forbidding ACE");
    return 1;
  }

  // Добавляем разрешающие ACE
  if (!AddAccessAllowedAce(pNewAcl, ACL_REVISION, GENERIC_READ, pSid))
  {
    PrintLastError("Error when adding a permissive ACE");
    return 1;
  }

  // Шаг 5: Установка нового ACL с помощью SetSecurityInfo
  if (SetNamedSecurityInfo((LPSTR) fileName, SE_FILE_OBJECT, DACL_SECURITY_INFORMATION, NULL,
                           NULL, pNewAcl, NULL) != ERROR_SUCCESS)
  {
    PrintLastError("Error when installing a new ACL");
    return 1;
  }
  printf("ACL changed successfully.\n");

  // Шаг 6: Получение имени пользователя по SID с использованием LookupAccountSid
  WCHAR userName[256];
  WCHAR domainName[256];
  DWORD userNameSize = sizeof(userName) / sizeof(WCHAR);
  DWORD domainNameSize = sizeof(domainName) / sizeof(WCHAR);
  SID_NAME_USE sidType;
  if (!LookupAccountSid(NULL, pSid, reinterpret_cast<LPSTR>(userName), &userNameSize,
                        reinterpret_cast<LPSTR>(domainName), &domainNameSize, &sidType))
  {
    PrintLastError("Error when getting the user name by SID");
    return 1;
  }
  printf("User: %S\\%S\n", domainName, userName);

  // Освобождение ресурсов
  if (hToken)
  {
    CloseHandle(hToken);
  }
  if (pTokenUser)
  {
    free(pTokenUser);
  }
  if (pSD)
  {
    LocalFree(pSD);
  }
  if (pNewAcl)
  {
    LocalFree(pNewAcl);
  }
  return 0;
}

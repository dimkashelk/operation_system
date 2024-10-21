#include <windows.h>
#include <aclapi.h>
#include <cstdio>
void PrintLastError(const char *msg)
{
  DWORD errorMessageID = GetLastError();
  if (errorMessageID != 0)
  {
    LPSTR messageBuffer = NULL;
    DWORD size = FormatMessageA(
      FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
      NULL, errorMessageID, 0, (LPSTR) &messageBuffer, 0, NULL);
    if (size)
    {
      printf("%s: %s\n", msg, messageBuffer);
    }
    else
    {
      printf("%s: Unknown error (Error Code: %lu)\n", msg, errorMessageID);
    }
    if (messageBuffer)
    {
      LocalFree(messageBuffer);
    }
  }
  else
  {
    printf("%s: No error code available.\n", msg);
  }
}
int DenyWriteAccess(CHAR *fileName)
{
  PSID pEveryoneSID = NULL;
  PACL pNewAcl = NULL;
  PSECURITY_DESCRIPTOR pSD = NULL;
  EXPLICIT_ACCESS ea;
  SID_IDENTIFIER_AUTHORITY SIDAuthWorld = SECURITY_WORLD_SID_AUTHORITY;
  PACL pOldAcl = NULL;
  if (GetNamedSecurityInfo(fileName, SE_FILE_OBJECT, DACL_SECURITY_INFORMATION, NULL, NULL, &pOldAcl, NULL, &pSD) !=
      ERROR_SUCCESS)
  {
    PrintLastError("Failed to retrieve security information");
    return 1;
  }
  if (!AllocateAndInitializeSid(&SIDAuthWorld, 1, SECURITY_WORLD_RID,
                                0, 0, 0, 0, 0, 0, 0, &pEveryoneSID))
  {
    PrintLastError("Failed to initialize SID");
    if (pSD)
    {
      LocalFree(pSD);
    }
    return 1;
  }
  ZeroMemory(&ea, sizeof(EXPLICIT_ACCESS));
  ea.grfAccessPermissions = FILE_GENERIC_WRITE;
  ea.grfAccessMode = DENY_ACCESS;
  ea.grfInheritance = NO_INHERITANCE;
  ea.Trustee.TrusteeForm = TRUSTEE_IS_SID;
  ea.Trustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;
  ea.Trustee.ptstrName = (LPTSTR) pEveryoneSID;
  if (SetEntriesInAcl(1, &ea, pOldAcl, &pNewAcl) != ERROR_SUCCESS)
  {
    PrintLastError("Failed to set entries in ACL");
    if (pSD)
    {
      LocalFree(pSD);
    }
    if (pEveryoneSID)
    {
      FreeSid(pEveryoneSID);
    }
    return 1;
  }
  if (SetNamedSecurityInfo(fileName, SE_FILE_OBJECT,
                           DACL_SECURITY_INFORMATION,
                           NULL, NULL, pNewAcl, NULL) != ERROR_SUCCESS)
  {
    PrintLastError("Failed to apply new ACL");
    if (pSD)
    {
      LocalFree(pSD);
    }
    if (pEveryoneSID)
    {
      FreeSid(pEveryoneSID);
    }
    if (pNewAcl)
    {
      LocalFree(pNewAcl);
    }
    return 1;
  }
  printf("Write access denied to everyone.\n");
  if (pSD)
  {
    LocalFree(pSD);
  }
  if (pEveryoneSID)
  {
    FreeSid(pEveryoneSID);
  }
  if (pNewAcl)
  {
    LocalFree(pNewAcl);
  }
  return 0;
}
int RevertPermissions(CHAR *fileName)
{
  if (SetNamedSecurityInfo(fileName, SE_FILE_OBJECT,
                           DACL_SECURITY_INFORMATION, NULL, NULL,
                           NULL, NULL) != ERROR_SUCCESS)
  {
    PrintLastError("Failed to revert security settings");
    return 1;
  }
  printf("Permissions reverted to default.\n");
  return 0;
}
int main()
{
  CHAR *fileName = "test_example.txt";
  HANDLE hFile = CreateFile(
    fileName,
    GENERIC_READ | GENERIC_WRITE,
    0,
    NULL,
    CREATE_ALWAYS,
    FILE_ATTRIBUTE_NORMAL,
    NULL);
  if (hFile == INVALID_HANDLE_VALUE)
  {
    PrintLastError("Failed to create or open the file");
    return 1;
  }
  printf("File successfully created or opened for reading and writing.\n");
  const char *data = "This is a test write.";
  DWORD bytesWritten;
  if (!WriteFile(hFile, data, (DWORD) strlen(data), &bytesWritten, NULL))
  {
    PrintLastError("Error writing to the file");
  }
  else
  {
    printf("File write successful.\n");
  }
  CloseHandle(hFile);
  if (DenyWriteAccess(fileName) != 0)
  {
    return 1;
  }
  hFile = CreateFile(
    fileName,
    GENERIC_WRITE,
    0,
    NULL,
    OPEN_EXISTING,
    FILE_ATTRIBUTE_NORMAL,
    NULL);
  if (hFile == INVALID_HANDLE_VALUE)
  {
    PrintLastError("Failed to open the file after applying new ACL");
  }
  else
  {
    if (!WriteFile(hFile, data, (DWORD) strlen(data), &bytesWritten, NULL))
    {
      PrintLastError("Write access denied as expected");
    }
    else
    {
      printf("Unexpected: Write access was not denied.\n");
    }
    CloseHandle(hFile);
  }
  if (RevertPermissions(fileName) != 0)
  {
    return 1;
  }
  return 0;
}

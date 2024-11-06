#include <sys/types.h>
#include <sys/msg.h>

#include <stdexcept>
#include <cstddef>
#include <cstring>
#include <type_traits>

key_t safe_ftok(const char * path, int pj_id);
int safe_msgget(key_t key, int msgflg);
void safe_msgctl(int msqid, int cmd, struct msqid_ds * buf);

template< typename buf_t >
using has_mtype = std::is_same< decltype(std::declval< buf_t >().mtype), long >;

template< typename buf_t >
std::enable_if_t< has_mtype< buf_t >::value, void >
safe_msgsnd(int msqid, const buf_t * msgp, int msgflg)
{
  if (msgsnd(msqid, msgp, sizeof(*msgp) - sizeof(msgp->mtype), msgflg) < 0)
  {
    throw std::runtime_error(std::strerror(errno)); 
  }
}

template< typename buf_t >
std::enable_if_t< has_mtype< buf_t >::value, const std::size_t >
safe_msgrcv(int msqid, buf_t * msgp, int msgflg)
{
  std::size_t recv = msgrcv(msqid, msgp, sizeof(*msgp) - sizeof(msgp->mtype), msgp->mtype, msgflg);
  if (recv != sizeof(*msgp) - sizeof(msgp->mtype))
  {
    throw std::runtime_error(std::strerror(errno));
  }
  return recv;
}

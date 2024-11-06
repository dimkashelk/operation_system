#include <cstddef>
#include <ostream>
#include <istream>

enum class opt_t
{
  ADD,
  SUB,
  MUL,
  DIV
};

enum class state_t
{
  SUCCES,
  FAILED
};

struct input_t
{
  long mtype;

  opt_t opt;
  double lhs;
  double rhs;
};

struct output_t
{
  long mtype;

  state_t state;
  double rhs;
};

struct msg_t
{
  long mtype;
  char mtext[256];
};

output_t calc(const input_t & input);

std::istream & operator>>(std::istream & in, input_t & rhs);

std::ostream & operator<<(std::ostream & out, const input_t & rhs);
std::ostream & operator<<(std::ostream & out, const output_t & rhs);
std::ostream & operator<<(std::ostream & out, const msg_t & msg);

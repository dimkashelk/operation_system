#include "msg.hpp"

#include <map>

output_t calc(const input_t & input)
{
  output_t output { .mtype = 'O' };
  if (input.rhs == 0.0 && input.opt == opt_t::DIV)
  {
    output.state = state_t::FAILED;
    return output;
  }

  switch (input.opt)
  {
    case opt_t::ADD:
      output.rhs = input.lhs + input.rhs;
      break;

    case opt_t::SUB:
      output.rhs = input.lhs - input.rhs;
      break;

    case opt_t::MUL:
      output.rhs = input.lhs * input.rhs;
      break;

    case opt_t::DIV:
      output.rhs = input.lhs / input.rhs;
      break;
  }
  output.state = state_t::SUCCES;

  return output;
}

std::istream & operator>>(std::istream & in, input_t & rhs)
{
  char sign;
  rhs.mtype = 'I';
  in >> rhs.lhs >> sign >> rhs.rhs;

  std::map< const char, opt_t > opt {
    {'+', opt_t::ADD},
    {'-', opt_t::SUB},
    {'*', opt_t::MUL},
    {'/', opt_t::DIV}
  };
  rhs.opt = opt.at(sign);

  return in;
}

std::ostream & operator<<(std::ostream & out, const input_t & rhs)
{
  std::map< opt_t, const char > opt {
    {opt_t::ADD, '+'},
    {opt_t::SUB, '-'},
    {opt_t::MUL, '*'},
    {opt_t::DIV, '/'}
  };
  return out << rhs.lhs << " " << opt.at(rhs.opt) << " " << rhs.rhs;
}
std::ostream & operator<<(std::ostream & out, const output_t & rhs)
{
  return out << rhs.rhs;
}

std::ostream & operator<<(std::ostream & out, const msg_t & msg)
{
  return out << "MSG: " << msg.mtext;
}

#include <iostream>
#include <string>

#include <Wt/Dbo/Dbo>

namespace dbo = Wt::Dbo;

class User {
public:
  enum Role {
    Visitor = 0,
    Admin = 1,
    Alien = 42
  };

  std::string name;
  std::string password;
  Role        role;
  int         karma;

  template<class Action>
  void persist(Action& a)
  {
    dbo::field(a, name,     "name");
    dbo::field(a, password, "password");
    dbo::field(a, role,     "role");
    dbo::field(a, karma,    "karma");
  }
};

int main(int argc, char *argv[])
{
#ifdef ZE_GAME
    std::cout << "Hello OpenZE!" << std::endl;
#elif ZE_SERVER
    std::cout << "Hello OZerver!" << std::endl;
#endif

#ifdef ZE_DEBUG
    std::cerr << "Da is was kaputt" << std::endl;
#endif
    return 0;
}

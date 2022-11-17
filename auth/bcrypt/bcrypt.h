#ifndef BCRYPT_H
#define BCRYPT_H

#include <string>

namespace bcrypt {

    std::string encrypt(const std::string & password , unsigned rounds = 10 );

    bool matches(const std::string & password, const std::string & hash);

}

#endif // BCRYPT_H

//
// Created by Calcite on 26-2-15.
//

#ifndef BASE64_H
#define BASE64_H
#include <string>


class base64 {
    public:

    static char base64_chars[];

    static std::string encode(const std::string& input);

    static std::string decode(const std::string& input);
};



#endif //BASE64_H

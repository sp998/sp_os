#include <io.h>

class Person{

private :
    char* name;
    int age;
public:
    Person(char* name,int age){
        this->name = name;
        this->age = age;
    }
    void print_info(){
        print("Name:");
        print(name);
        print("\n");
        print("Age:");
        print_number(age);
        print("\n");
    }
};

extern "C" void cpp_fun(){
    Person person("John Doe",30);
    person.print_info();
}
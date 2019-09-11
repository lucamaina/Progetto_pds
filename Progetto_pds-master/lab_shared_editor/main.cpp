#include <iostream>

#include <vector>
#include <algorithm>
#include "NetworkServer.h"
#include "SharedEditor.h"

int main() {
    std::cout << "Hello, World!" << std::endl;

    std::vector<int> v{10,12,13};
    std::vector<int> w;
    std::cout << w.begin().base() << std::endl;

    v.insert(v.end(), 50);
    std::cout << v.end().base() << std::endl;
    //std::cout << v.end().base() << std::endl;
    std::for_each(v.begin(), v.end(), [] (auto i) {std::cout << " - " << i;} );

    auto vit = v.begin();
    std::cout << *(vit) << std::endl;
    v.insert(vit+1, 20);
    std::for_each(v.begin(), v.end(), [] (auto i) {std::cout << " - " << i;} );
    std::cout << std::endl;


    NetworkServer network;

    SharedEditor ed1(network);
    SharedEditor ed2(network);

    ed1.localInsert(0, 'c');
    ed1.localInsert(1, 'a');
    ed1.localInsert(2, 't');
    network.dispatchMessages();
    std::cout<<"ed1: "<<ed1.to_string()<<std::endl;
    std::cout<<"ed2: "<<ed2.to_string()<<std::endl;

    ed1.localInsert(1,'h');
    //network.dispatchMessages();
    std::cout<<"ed1: "<<ed1.to_string()<<std::endl;
    std::cout<<"ed2: "<<ed2.to_string()<<std::endl;


    ed2.localErase(1);
    network.dispatchMessages();
    std::cout<<"ed1: "<<ed1.to_string()<<std::endl;
    std::cout<<"ed2: "<<ed2.to_string()<<std::endl;

    return 0;
}
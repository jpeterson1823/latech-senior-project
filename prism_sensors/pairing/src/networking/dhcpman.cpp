#include "networking/dhcpman.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib>
#include <pwd.h>

DHCPLease::DHCPLease(std::string ip4str, std::string macstr) : ip4(ip4str), mac(macstr) {}
DHCPLease::DHCPLease(Mac mac, IP4 ip4) : ip4(ip4), mac(mac) {};
IP4* DHCPLease::getIP4() { return &ip4; }
Mac* DHCPLease::getMac() { return &mac; }
std::string DHCPLease::toString() {
    std::string s = "";
    s += mac.toString();
    s += ';';
    s += ip4.toString();
    return s;
}


const std::string DHCPMan::LEASE_FILE_PATH = std::string(getpwuid(getuid())->pw_dir) + "/.prism/module.leases";
DHCPMan::DHCPMan() { 
    // fill availableIP4s with valid ip4str's (x.x.x.3-254)
    IP4 ip4("192.168.0.3");
    while (ip4.octets[3] < 255) {
        availableIP4s.push_back(ip4.toString());
        ip4.octets[3] += 1;
    }

    // load lease file
    loadLeaseFile();
}

DHCPMan::~DHCPMan() {
    // delete all leases from memory
    for (std::map<Mac*, IP4*>::iterator it = leases.begin(); it != leases.end(); ++it) {
        delete(it->first);
        delete(it->second);
    }
}

/*/
 * Fork to python process that will query the db and create a formatted file of the currently
 * paired sensors IP4:MAC information. Wait for process to terminate before continuing.
/*/
void DHCPMan::forkToLeaseLoader() {
    // fork the process
    pid_t pid = fork();

    // if this process is the child process, execute db lease loader script
    if (pid == 0) {
        std::cout << "Forked to dbLeaseLoader.py:" << std::endl;
        system("source ../.venv/bin/activate && python3 ../dbLeaseLoader.py");
        exit(0);
    }
    
    // otherwise, wait for child process to finish
    int status;
    pid_t child;
    for (;;) {
        child = wait(&status);
        if (child == -1)
            break;
        else {
            if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
                std::cout << "DHCPMan.loadLeasesFile() process failed!\n";
                std::cout << "  process pid: " << child << '\n';
                std::cout << "  exit code  : " << WEXITSTATUS(status) << std::endl;
                exit(1);
            }
        }
    }
}

void DHCPMan::loadLeaseFile() {
    // fork to lease loader script
    forkToLeaseLoader();

    // open lease file
    std::ifstream leaseIfs(LEASE_FILE_PATH);
    if (!leaseIfs.is_open()) {
        std::cout << "[CRIT ERR] DHCPMan failed to open lease file @ ";
        std::cout << LEASE_FILE_PATH << std::endl;
        return;
    }

    // parse lease file into this->leases
    std::string line;
    while (leaseIfs.good() && leaseIfs.peek() != EOF) {
        // get the current line
        leaseIfs.getline(line.data(), 256);
        std::cout << line << std::endl;
        // split the string into an ip4str and macstr
        std::size_t delimitIndex = line.find(';');
        std::string ip4str = line.substr(0,delimitIndex);
        std::string macstr = line.substr(delimitIndex+1, line.size() - delimitIndex - 1);

        std::cout << ip4str << ' ' << macstr << std::endl;
        // find the location of the paired ip4str in availableIP4s
        std::vector<std::string>::iterator it; 
        for (it = availableIP4s.begin(); it != availableIP4s.end(); ++it) {
            std::cout << *it << std::endl;
            if (it->compare(ip4str) == 0)
                break;
        }
        availableIP4s.erase(it);

        // add pair to leases
        std::cout << macstr << std::endl;
        std::cout << ip4str << std::endl;
        leases.insert({new Mac(macstr), new IP4(ip4str)});
        std::cout << "fixed" << std::endl;
    }
}

IP4* DHCPMan::genNewIP4() {
    std::string ipstr = *(availableIP4s.begin()+1);
    availableIP4s.erase(availableIP4s.begin()+1);
    return new IP4(ipstr);
}

DHCPLease* DHCPMan::lease(Mac* mac) {
    // generate new IP addr
    IP4* ip4 = genNewIP4();

    // add MAC:IP4 pair to map
    leases.insert({mac, ip4});

    // generate lease object and return
    return new DHCPLease(ip4->toString(), mac->toString());
}

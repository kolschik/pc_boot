#pragma once

class can_api : public boot_api{
public:
    can_api();

    int open();
    int write();
    int read();
    int erase();
    int lock();
};
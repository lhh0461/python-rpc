#ifndef __NET_BUFF__
#define __NET_BUFF__

class Buffer
{
public:
    Buffer();
    ~Buffer();
private:
    void Expand();
private:
    char *buf;
    size_t len;
};

#endif //__NET_BUFF__


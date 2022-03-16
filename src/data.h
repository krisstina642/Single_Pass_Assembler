#ifndef DATA_H_
#define DATA_H_


class Data
{
private:
    int size; // broj bajtova 
    int data;


public:
    Data(int size);
    Data(int size, int data);
    void addData(int data);
    void subData(int data);
    int getData();
    void setData(int data);
    int getSize();
    void setSize(int size);
};



#endif

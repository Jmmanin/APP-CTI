//header for the packet queue module

void Q_Init(int new_size);

int Q_getPacketSize();

int Q_isEmpty();

int Q_size();  

void Q_peek(char *output_buff);

void Q_addData(char *data);

void Q_removeData(char *output_buff);
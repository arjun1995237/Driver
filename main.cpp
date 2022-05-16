#include <stdio.h>
#include<iostream>
#include<cstring>
#include <fstream>

#include <unistd.h>


// Linux headers
#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()
#define window 150
int count=0;
//To write to a serial port, you write to the file. To read from a serial port, you read from the file
using namespace std;
void convert_char_arr(string s,char* p,int& n){

   
    // declaring character array : p
    
 
    int i;
    for (i = 0; i < s.length(); i++) {
        p[i] = s[i];
        n++;
        if (p[i]=='\n')
        	break;
    }

}
void change_parameter(int serial_port,double value){
//if prefix is 'P',then the value is proportional gain
//if prefix is 'I',then the value is integral gain
//if prefix is 'D',then the value is derivative gain
//if prefix is 'S',then the value is reference speed

string val=to_string(value)+'\n';
cout<<"sending the following--->"<<val<<"\n";
int size=0;
char* char_array=new char[val.length()];
convert_char_arr(val,char_array,size);
write(serial_port,char_array,size);
usleep(1000000); 
delete char_array;

}
/*bool tune_controller(int serial_port,double Kp,double Ki,double Kd,double set_point){
    char c;
 string Kp_val="P";
    Kp_val=Kp_val+to_string(Kp); 
 string Ki_val="I";
    Ki_val=Ki_val+to_string(Ki); 
 string Kd_val="D";
    Kd_val=Kd_val+to_string(Kd); 
 string speed="S";
    speed=speed+to_string(set_point); 
cout<<Kp_val<<" "<<Ki_val<<" "<<Kd_val<<" "<<speed;
Kp_val=Kp_val+'\n';
Ki_val=Ki_val+'\n';
Kd_val=Kd_val+'\n';
speed=speed+'\n';
int p=0;
char* kp=new char[Kp_val.length()];
convert_char_arr(Kp_val,kp,p);
write(serial_port,kp ,p);
    usleep(1000000); 
    p=0;  
char* ki=new char[Ki_val.length()];
convert_char_arr(Ki_val,ki,p);
write(serial_port, ki, p);
    usleep(1000000); 
    p=0;
char* kd=new char[Kd_val.length()];
convert_char_arr(Kd_val,kd,p);    
write(serial_port, kd,p);
    usleep(1000000);        
	p=0;
char* speeed=new char[speed.length()];
convert_char_arr(speed,speeed,p);
     


write(serial_port, speeed, p);
    usleep(1000000);        

delete kp;
delete ki;
delete kd;
delete speeed;
return true;
}*/
int main(int argc, char *argv[]){
    int n=0;
    ofstream outfile;
   outfile.open("example.txt");
    int serial_port=open(argv[1],O_RDWR);
cout<<argv[1]<<" ";
    if(serial_port<0){
        printf("Error %i from open: %s\n", errno, std::strerror(errno));
    }
//One of the common errors you might see here is errno = 2, and strerror(errno) returns No such file or directory. Make sure you have the right path to the device and that the device exists!
//
//Another common error you might get here is errno = 13, which is Permission denied. This usually happens because the current user is not part of the dialout group. Add the current user to the dialout group with:
//sudo adduser $USER dialout.

//configurations

    struct termios tty;
    if(tcgetattr(serial_port, &tty) != 0) {
    printf("Error %i from tcgetattr: %s\n", errno, std::strerror(errno));
    }
tty.c_cflag &= ~PARENB; // Clear parity bit, disabling parity (most common)
tty.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication (most common)
tty.c_cflag &= ~CSIZE; // Clear all the size bits, then use one of the statements below
tty.c_cflag |= CS8; // 5 bits per byte
tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control (most common)
tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)
tty.c_lflag &= ~ICANON;
tty.c_lflag &= ~ECHO; // Disable echo
tty.c_lflag &= ~ECHOE; // Disable erasure
tty.c_lflag &= ~ECHONL; // Disable new-line echo
tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes
tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed
   
tty.c_cc[VTIME] = 0;   // This will make read() always wait for bytes (exactly how many is determined by VMIN), so read() could block indefinitely.
tty.c_cc[VMIN] = 1;


//B0,  B50,  B75,  B110,  B134,  B150,  B200, B300, B600, B1200, B1800, B2400, B4800, B9600, B19200, B38400, B57600, B115200, B230400, B460800
//

cfsetispeed(&tty, B9600);
cfsetospeed(&tty, B9600);
if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
    printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
}
//unsigned char msg[] = { 'H', 'e', 'l', 'l', 'o', '\r' };
//
//write(serial_port, msg, sizeof(msg));sleep(0.1);
char option;int num_bytes;


string result="";

while(1){

cout<<"Do you want to tune/display/exit? [1 - Tune /0 - Display/2-moveF /3-moveB/ 4-stop/ -1 -Exit]\n";
cin>>option;

if(option=='0'){


write(serial_port,&option,1);
// Read bytes. The behaviour of read() (e.g. does it block?,
// how long does it block for?) depends on the configuration
// settings above, specifically VMIN and VTIME
 char* read_buf=new char;
while(1){

num_bytes = read(serial_port,read_buf,1);

 if (num_bytes < 0) {
      printf("Error reading: %s", strerror(errno));
      return 1;
  }


    if(*read_buf!='\n')
        result=result+char(*read_buf);
    else
        break;

}


// Allocate memory for read buffer, set size according to your needs

// num_bytes is the number of bytes read. n may be 0 if no bytes were received, and can also be negative to signal an error.




   // write inputted data into the file.
cout<<to_string(count)<<","<<result<<endl;
   outfile<<to_string(count)<<","<<result<<endl;
    count++;
    result="";
delete read_buf;

}
else if(option=='1'){


 double Kp,Ki,Kd,speed;
    cout<<"Enter Kp, Ki, Kd values ?\n";
    cin>>Kp;
    cin>>Ki;
    cin>>Kd;
    cout<<"Enter speed ?\n";
    cin>>speed;

write(serial_port,&option,1);
   cout<<Kp<<" "<<Ki<<" "<<Kd<<"\n";
    change_parameter(serial_port,Kp);
    change_parameter(serial_port,Ki);
    change_parameter(serial_port,Kd);
    change_parameter(serial_port,speed);
cout<<"Done parameter tuning\n";

}
else if(option=='2'|| option=='3' || option=='4'){//forward
 
write(serial_port,&option,1);

}

else{

 cout<<"Invalid option, try again!!!\n";
}
}


return 1;


}


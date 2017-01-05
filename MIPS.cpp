#include<iostream>
#include<string>
#include<vector>
#include<bitset>
#include<fstream>
using namespace std;
#define ADDU 1
#define SUBU 3
#define AND 4
#define OR  5
#define NOR 7
#define MemSize 65536 // memory size, in reality, the memory size should be 2^32, but for this lab, for the space reason, we keep it as this large number, but the memory is still 32-bit addressable.


class RF//register file
{
    public:
        bitset<32> ReadData1, ReadData2; 
     	RF()
    	{ 
          Registers.resize(32);  
          Registers[0] = bitset<32> (0);  
        }
	
        void ReadWrite(bitset<5> RdReg1, bitset<5> RdReg2, bitset<5> WrtReg, bitset<32> WrtData, bitset<1> WrtEnable)
        {
            if(WrtEnable == 0)//read
            {
                size_t num1 = 0;
                size_t num2 = 0;
                
                num1 = RdReg1.to_ulong();//get RdReg1's number, 0 to 31,
                num2 = RdReg2.to_ulong();//get RdReg2's number, 0 to 31,
                
                //register $0 is always 0
                ReadData1 = (num1 == 0) ? ReadData1.reset(): Registers[num1];
                ReadData2 = (num2 == 0) ? ReadData2.reset(): Registers[num2];
            }
            else//write
            {
                size_t num1 = 0;
                num1 = WrtReg.to_ulong();//get RdReg1's number, 0 to 31, ??
                
                //register $0 is always 0
                Registers[num1] = (num1 == 0) ? Registers[num1].reset(): WrtData;// ???
                //printf("WrtReg%x %x %x\n", num1,Registers[num1].to_ulong(),WrtReg.to_ulong());
            }// implement the funciton by you.
        }
		 
	void OutputRF()
             {
               ofstream rfout;
                  rfout.open("RFresult.txt",std::ios_base::app);
                  if (rfout.is_open())
                  {
                    rfout<<"A state of RF:"<<endl;
                  for (int j = 0; j<32; j++)
                      {        
                        rfout << Registers[j]<<endl;
                      }
                     
                  }
                  else cout<<"Unable to open file";
                  rfout.close();
               }     
	private:
            vector<bitset<32> >Registers;
	
};

class ALU
{
      public:
             bitset<32> ALUresult;
             bitset<32> ALUOperation (bitset<3> ALUOP, bitset<32> oprand1, bitset<32> oprand2)
             {   
                 // implement the ALU operations by you.
                 size_t i = 0;
                 size_t num1 = 0;
                 for (i = 0; i<ALUOP.size(); ++i)
                     num1 |= (ALUOP.test(i) << i);
                 
                 switch(num1)
                 {
                     case 0x01://addu, 21hex
                     {
                         size_t sum = 0;
                         
                         sum = oprand1.to_ulong() + oprand2.to_ulong(); //ALUresult = oprand1[0]  + oprand2[0];
                         bitset<32> ALUresult1 (sum);
                         ALUresult = ALUresult1;
                         break;
                     }
                     case 0x03://subu, 23hex
                     {
                         size_t sum = 0;
                         
                         sum = oprand1.to_ulong() - oprand2.to_ulong(); //ALUresult = oprand1[0]  - oprand2[0];
                         ALUresult = bitset<32>(sum);
                         break;
                     }
                     case 0x04://and, 24hex
                     {
                         ALUresult = oprand1 & oprand2;
                         break;
                     }
                     case 0x05://or, 25hex
                     {
                         ALUresult = oprand1 | oprand2;
                         break;
                     }
                     case 0x07://nor, 27hex
                     {
                         ALUresult = oprand1 | oprand2;
                         ALUresult = ALUresult.flip();//nor=~(a | b)
                         break;
                     }
                     default:
                         break;
                 }
                 
                 return ALUresult;
             }
};

class INSMem
{
      public:
          bitset<32> Instruction;
          INSMem()
          {       IMem.resize(MemSize); 
                  ifstream imem;
                  string line;
                  int i=0;
                  imem.open("imem.txt");
                  if (imem.is_open())
                  {
                  while (getline(imem,line))
                     {      
                         IMem[i] = bitset<8>(line);
                        i++;
                     }
                     
                  }
                  else cout<<"Unable to open file";
                  imem.close();
                     
                  }
                  
          bitset<32> ReadMemory (bitset<32> ReadAddress) 
          {
               // implement by you. (Read the byte at the ReadAddress and the following three byte).
              size_t address;
              size_t InsULong;
              
              address = ReadAddress.to_ulong();
              InsULong = (IMem[address].to_ulong()<<24) | (IMem[address + 1].to_ulong()<<16) \
              | (IMem[address + 2].to_ulong()<<8) | IMem[address + 3].to_ulong();
              Instruction = bitset<32>(InsULong);
              
              return Instruction;
          }
      
      private:
           vector<bitset<8> > IMem;
      
};
      
class DataMem    
{
      public:
          bitset<32> readdata;  
          DataMem()
          {
             DMem.resize(MemSize); 
             ifstream dmem;
                  string line;
                  int i=0;
                  dmem.open("dmem.txt");
                  if (dmem.is_open())
                  {
                  while (getline(dmem,line))
                       {      
                        DMem[i] = bitset<8>(line);
                        i++;
                       }
                  }
                  else cout<<"Unable to open file";
                  dmem.close();
          
          }  
          bitset<32> MemoryAccess (bitset<32> Address, bitset<32> WriteData, bitset<1> readmem, bitset<1> writemem) 
          {
              // implement by you.
              if(writemem.test(0) == true)//write enabled
              {
                  //printf("writemem%x\n", writemem.to_ulong());
                  size_t address;
                  size_t dataULong;
                  int i,j;
                  
                  address = Address.to_ulong();
                  
                  dataULong = WriteData.to_ulong();//printf("address%x dataULong%x\n", address, dataULong);
                  
                  for (i = 0; i < 4; i ++)
                  {
                      j = 3 - i;
                      DMem[address + i] = bitset<8>(dataULong >> (j * 8));    // write into memory
                  }
              }
              
              if(readmem.test(0) == true)//read enabled
              {
                  //printf("readmem%x\n", readmem.to_ulong());
                  unsigned long address;
                  unsigned long dataULong;
                  
                  address = Address.to_ulong();
                  //printf("%x\n",address);
                  dataULong = (DMem[address].to_ulong()<<24) | (DMem[address + 1].to_ulong()<<16) \
                  | (DMem[address + 2].to_ulong()<<8) | DMem[address + 3].to_ulong();
                  //printf("DMem[0]%x\n",dataULong);
                  readdata = bitset<32>(dataULong);    //read from memory
              }
              
              return readdata;
          }   
                     
          void OutputDataMem()
          {
               ofstream dmemout;
                  dmemout.open("dmemresult.txt");
                  if (dmemout.is_open())
                  {
                  for (int j = 0; j< 1000; j++)
                       {     
                        dmemout << DMem[j]<<endl;
                       }
                     
                  }
                  else cout<<"Unable to open file";
                  dmemout.close();
               
               }             
      
      private:
           vector<bitset<8> > DMem;
      
};  


   
int main()
{
    RF myRF;
    ALU myALU;
    INSMem myInsMem;
    DataMem myDataMem;

    bitset<32> ReadAddress (0);
    
    while (1)
	{
        if(ReadAddress.to_ulong() > 65536)
        {
            break;
        }
        
        // Fetch
        bitset<32> Instruction;
       
        Instruction = myInsMem.ReadMemory(ReadAddress);
        size_t InstructionUInt = (size_t)Instruction.to_ulong();
        printf("InstructionUInt%x %x\n", InstructionUInt, ReadAddress.to_ulong());
		// If current insturciton is "11111111111111111111111111111111", then break;
        if(InstructionUInt == 0xFFFFFFFF)
        {
            break;
        }
        else
        {
            // decode(Read RF)
            bitset<5> RdReg1 ((InstructionUInt >> 21)&0x1F);
            bitset<5> RdReg2 ((InstructionUInt >> 16)&0x1F);
            bitset<5> WrtReg ((InstructionUInt >> 11)&0x1F);
            bitset<1> WrtEnable (0);
            bitset<32> WrtData (0);
            
            myRF.ReadWrite(RdReg1, RdReg2, WrtReg, WrtData, WrtEnable);
            
          // Execute
            bitset<3> ALUOP;
            bitset<32> ALUresult;
            size_t opcode = InstructionUInt >> 26;
            //printf("%x\n", opcode);
            
            switch(opcode)
            {
                case 0x00://R-type
                {
                    ALUOP = bitset<3>(InstructionUInt);
                    ALUresult = myALU.ALUOperation(ALUOP, myRF.ReadData1, myRF.ReadData2);
                    break;
                }
                case 0x09: //addiu, I-type
                {
                    ALUOP = bitset<3>(0x01);
                    bitset<32> imme (InstructionUInt&0xFFFF);//zero extended
                    ALUresult = myALU.ALUOperation(ALUOP, myRF.ReadData1, imme);
                    break;
                }
                case 0x23: //lw, I-type
                case 0x2B: //sw, I-type
                {
                    size_t immediate = InstructionUInt&0xFFFF;
                    if(((immediate>>15)&0x01) == 1)
                    {
                        immediate = immediate | 0xFFFF0000;//sign extended
                    }
                    bitset<32> imme1 (immediate);
                    ALUOP = bitset<3>(0x01);
                    ALUresult = myALU.ALUOperation(ALUOP, myRF.ReadData1, imme1);
                    //printf("%x\n", ALUresult.to_ulong());
                    break;
                }
                default:
                    break;
            }
           
            
            // Read/Write Mem
            bitset<32> readdata;
            bitset<32> readMemAddr (0);
            bitset<32> WriteData (0);
            bitset<1> readmem;
            bitset<1> writemem;
            
            if(opcode == 0x23)  //lw, I-type
            {
                readmem = bitset<1> (1);
                writemem = bitset<1> (0);
                if(ALUresult.to_ulong() > 65535)
                {
                    printf("access violation!\n");
                    return 0;
                }
                readdata = myDataMem.MemoryAccess(ALUresult, WriteData, readmem, writemem);
                //printf("readdata.to_ulong()%x\n", readdata.to_ulong());
            }
            if(opcode == 0x2B)  //sw, I-type
            {
                readmem = bitset<1> (0);
                writemem = bitset<1> (1);
                WriteData = myRF.ReadData2;
                if(ALUresult.to_ulong() > 65535)
                {
                    printf("access violation!\n");
                    return 0;
                }
                printf("sw%x %x\n", ALUresult.to_ulong(), WriteData.to_ulong());
                myDataMem.MemoryAccess(ALUresult, WriteData, readmem, writemem);
            }
            
            // Write back to RF
            if(opcode == 0x00)  //R-type
            {
                WrtEnable = bitset<1> (1);
                printf("WrtRegR%x \n", WrtReg.to_ulong());
                myRF.ReadWrite(RdReg1, RdReg2, WrtReg, ALUresult, WrtEnable);
            }
            else if(opcode == 0x23)           //lw
            {
                WrtEnable = bitset<1> (1);
                printf("WrtRegI%x \n", RdReg2.to_ulong());
                myRF.ReadWrite(RdReg1, RdReg2, RdReg2, readdata, WrtEnable);
                //printf("%x\n", readdata.to_ulong());
            }
            else if(opcode == 0x09)           //addiu
            {
                WrtEnable = bitset<1> (1);
                myRF.ReadWrite(RdReg1, RdReg2, RdReg2, ALUresult, WrtEnable);
                //printf("%x\n", readdata.to_ulong());
            }
            
            //pc
            size_t pcAddr = ReadAddress.to_ulong();
            if(opcode == 0x04)//beq
            {
                if(myRF.ReadData1 == myRF.ReadData2)
                {
                    size_t immediate = InstructionUInt&0xFFFF;//immediate, 16bits
                    if(((immediate>>15)&0x01) == 1)
                    {
                        immediate = immediate | 0xFFFF0000;   //sign extended
                    }
                    immediate = immediate<<2;                 //{SignExtendImm, 00}
                    
                    pcAddr += (4 + immediate);
                }
                else
                {
                    pcAddr += 4;
                }
            }
            else if(opcode == 0x02)//j PC  ← {PC+4[31:28], address, 00}
            {
                size_t jAddr = (InstructionUInt << 2);
                jAddr = jAddr & 0xFFFFFFF;
                pcAddr = ((pcAddr + 4) & 0xF0000000) | jAddr;
            }
            else
            {
                pcAddr += 4;
            }
            ReadAddress = bitset<32>(pcAddr);
        }
    
		myRF.OutputRF();// dump RF;
    }
    
    myDataMem.OutputDataMem(); // dump data mem
    
    printf("hello, world!\n");
      
    return 0;
        
}

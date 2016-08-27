/* Nikhil and  Nishant */

#include "tobinary.h"

//range from -32768 to 32767
int chkint(char* str)
{
	int i=0;
	if(str[i]=='-' || str[i] == '+')
		i=1;
	while(str[i]!='\0'){		
		if(!isdigit(str[i])){		
			return 0;
		}
		i++;
	}
	int a = atoi(str);
	//printf("%d",a);
	if(a>=(-32768)&&a<=(32767))
		return 1;
	return 0;	
}

// range from  2^-14=0.00006103515 to (2-2^-10)*2^15=65504
int chkfloat(char* str)
{
	double f = atof(str);
	int i=0,count=0;
	if(str[i]=='-' || str[i] == '+')
		i=1;
	while(str[i]!='\0'){		
		if(!isdigit(str[i])){
			if(str[i]!='.')
				return 0;
			else{
				count++;
				if(count == 1)
					;
				else 
					return 0;
			}
		}
		i++;
	}

	if(f==0)
		return 1;
	if( (f>=0.00006103515&&f<=65504) || (f<=-0.00006103515&&f>=-65504))
		return 1;
	return 0;
}


char * dec2bin(char *str, char *pointer)
{
   int val, c, d, count=0;
   val = atoi(str);
   
   if (pointer == NULL)
      exit(EXIT_FAILURE); 
   for ( c = 15 ; c >= 0 ; c-- )
   {
      d = val >> c; 
      if ( d & 1 )
         *(pointer+count) = 1 +'0';
      else
         *(pointer+count) = 0 + '0'; 
      count++;
   }
   *(pointer+count) = '\0'; 
   return  pointer;	
}

char* flt2bin(char *str,char *pointer)
{
	int i=0,j,k,ival,iExp;
	char exp2[10];
	double fl;	
	char exponent[6];
   	char etemp2[6];
	char sign;
	char eTemp[10], mTemp[10]; 
	//char temp[11];
  	//sign bit
	if(str[i] == '-'){
   	  	sign = '1';
   	 	i++;
	}
    else
   		sign = '0';
	
	for( k=0;i<strlen(str) && str[i]!='.';i++,k++ )
		eTemp[k] = str[i];
		eTemp[k]='\0';
	for(j=0;i<=strlen(str);i++,j++)		
		mTemp[j]=str[i];
		eTemp[j+1]='\0';
	
	//making exponent 	
	ival = atoi(eTemp);
	fl = atof(mTemp);
	if(ival==0 && fl==0.0){//if str == 0
		for(k=0;k<5;k++)
			exponent[k]='0';
	}
	else{	//for 0-inf
	iExp = 15 + logbase(ival, 2);//for 0.xxx iExp = 30
	itoa( iExp, etemp2, 2 );// etemp2 is the iExp in binary
	for(k=0;k<5;k++)
		exponent[k]='0';
	for(k=4; k>=0; k--)
		exponent[k]=etemp2[k];
	}
	//making mantisa
	
	itoa(ival, exp2 ,2);	
	//writing to pointer
	pointer[0]=sign;
	for(i=0; i<=4; i++)
		pointer[i+1] = exponent[i];
	//printf("exponent: %s \npointer: %s \nintPart: %s\n",exponent,pointer,exp2);//
	//printf("length exp: %d\n",strlen(exp2));//
	for( j=1;j<strlen(exp2);j++)
	{
		//printf("int part to binary char: %c \n",exp2[j]);//		
		pointer[i+j] = exp2[j];
		//printf("pointer with half-mantisa: %s \n",pointer);//
	}	

	
	i+=j;
	//	printf("i at float part: %d\n",i);//
	while(fl!=0.000000 && i<16)
	{
		fl*=(double)2;	
		pointer[i++]= (int)fl + '0';
		fl-=(int)fl;		
		//printf("%lf\n",fl);	//		
	}
	//printf("\n%d\n",i);//
	for( ; i<16;i++)
		pointer[i]='0';
		pointer[16]='\0';
	
	
//	printf("%d %s\n",iExp, exp2);
//	printf("%s %s %s",eTemp,mTemp,str);  
    return  pointer;	
}

int logbase (int y, int b)
{
	if(y==0)
	return 15;
	else{
		int lg;
		lg = log10(y)/log10(b);
		return(lg);
	  }
} 

// Implementation of itoa()
char* itoa(int num, char* str, int base)
{
    int i = 0;
    int isNegative = 0;
 
    /* Handle 0 explicitely, otherwise empty string is printed for 0 */
    if (num == 0)
    {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }
 
    // In standard itoa(), negative numbers are handled only with 
    // base 10. Otherwise numbers are considered unsigned.
    if (num < 0 && base == 10)
    {
        isNegative = 1;
        num = -num;
    }
 
    // Process individual digits
    while (num != 0)
    {
        int rem = num % base;
        str[i++] = (rem > 9)? (rem-10) + 'a' : rem + '0';
        num = num/base;
    }
 
    // If number is negative, append '-'
    if (isNegative)
        str[i++] = '-';
 
    str[i] = '\0'; // Append string terminator
 
    // Reverse the string
    reverse(str, i);
 
    return str;
} 

/* A utility function to reverse a string  */
void reverse(char str[], int length)
{
    int start = 0;
    int end = length -1;
    char temp;
    while (start < end)
    {
        //swap(*(str+start), *(str+end));
	temp = *(str+start);
   	 *(str+start) = *(str+end);
   	 *(str+end) = temp;
	// no change
        start++;
        end--;
    }
}  

#include<stdio.h>
int main(){
int height=10,width=20,i,j;

//i represents height (row) , j represents column (width)
for(i=0;i<height;i++){
	for(j=0;j<width;j++){
		if(i==0 || i==height-1 || j==0 || j==width-1){
			printf("#");
		
}
		else{
			printf(" ");
}

}
	printf("\n");


}
return 0;





}

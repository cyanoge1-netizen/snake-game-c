#include<stdio.h>
int main(){
int height=10,width=20,i,j;
int x=10,y=5; //x represents column (left to right) and y represents Row (up to down)

//i represents height (row) , j represents column (width)
for(i=0;i<height;i++){
	for(j=0;j<width;j++){
		if(i==0 || i==height-1 || j==0 || j==width-1){
			printf("#");


}

		else if(i==y && j==x){
//prints snake head

			printf("0");
 
} 
		else{
			 //prints space inside box
			printf(" ");
 
}

}
	// starts new line after each box line
	printf("\n");


}
return 0;




//program ends here
}

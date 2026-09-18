#include<stdio.h>
int main(){
int height=10,width=20,i,j;
int x=10,y=5; //x represents column (left to right) and y represents Row (up to down)
char move;
//i represents height (row) , j represents column (width)
while(1){
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
//input and state update
	printf("Move (w/a/s/d, q to  exit): ");
	scanf("%c",&move);
	if(move == 'w')
		y--; //going up
	else if(move == 's')
		y++; //going down
	else if(move == 'd')
		x++; //going right
	else if(move == 'a')
		x--; //going left


}
return 0;




//program ends here
}

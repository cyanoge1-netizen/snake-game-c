#include<stdio.h>
#include<stdlib.h>
int main(){
int height=10,width=20,i,j;
int x=10,y=5; //x represents column (left to right) and y represents Row (up to down)
char move;
//i represents height (row) , j represents column (width)
while(1){
//screen clearing
system("clear");
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
//input and  buffer flash
	
	printf("Move (w/a/s/d, q to  exit): ");
	if( scanf(" %c",&move) != 1){
		break;
}
	//extra newline clear
	while(getchar() != '\n');

	// statement update & exit checking
	if(move == 'q' || move == 'Q'){
		printf("Exiting Game...\n");
		break;
}
	else if(move == 'w'|| move == 'W')
		y--; //going up
	else if(move == 's' || move == 'S')
		y++; //going down
	else if(move == 'd' || move == 'D')
		x++; //going right
	else if(move == 'a' || move == 'A')
		x--; //going left


}
return 0;




//program ends here
}

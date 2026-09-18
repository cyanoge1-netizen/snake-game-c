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
	else if(move == 'w'|| move == 'W' || move == '2')
		y--; //going up
	else if(move == 's' || move == 'S' || move == '8')
		y++; //going down
	else if(move == 'd' || move == 'D' || move == '6')
		x++; //going right
	else if(move == 'a' || move == 'A' || move == '4')
		x--; //going left

	// colision logic
	if(x <= 0 || x >= width-1 || y <= 0 || y >= height-1){
		system("clear");
		printf("\n Game Over !  Youve Hit the Wall !! \n\n");
		break;
}
}
return 0;




//program ends here
}


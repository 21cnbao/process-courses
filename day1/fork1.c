main()
{
	fork();
	printf("hello\n");
	fork();	
	printf("hello\n");
	while(1);
}

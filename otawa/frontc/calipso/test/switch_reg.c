
void main(void) {
	char c;
	int i;
	
	/* very simple */
	switch(c) {
	case 'a':
		i++;
		break;
	case 'b':
		i = 0;
		break;
	case 'c':
		i--;
		break;
	default:
		i = 0;
	}
	
	/* a bit more complex */
	switch(c) {
	case 'a':
		i++;
		break;
	case 'b': {
			if(i == 0)
				i++;
			else
				i = 0;
			break;
		}
	case 'c':
		if(i == 0) {
			i++;
			break;
		}
		else {
			i--;
			break;
		}
	default:
		i = 0;
	}
	
}

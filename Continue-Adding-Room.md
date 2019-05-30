# Continue Adding Room

This function determine if the generation should continue or not.

A generation will naturally stop if there are no door left with no room connected.\
But you could want to have doors with no room after in your generation.\
So you have to return false in this function to stop the generation.

In output, you have to return true to continue the generation, or false to stop it.

Here an example where we let the generation end naturally (be carefull of infinite loop):\
[Insert image here]
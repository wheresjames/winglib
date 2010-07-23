
function _init()
{
	// Spawn child
	_self.spawn(	0,												// Don't wait for return
					".",											// Child of this thread
					"child_0",										// Name
					"_self.alert( \"This is another thread\" );",	// The code
					0												// Prev param not a filename
				);

	return 0;
}

function _idle()
{
	_self.alert( "Waiting..." );

	return 1;
}


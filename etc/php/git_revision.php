<?php

	function git_revision_log( $dir )
	{
		$revfile = 'gitrev-' . md5( $dir ) . '.txt';
		$log = 'No Changes';

		$rev_prev = 0;
		if ( is_file( $revfile ) )
			$rev_prev = file_get_contents( $revfile );

		$od = getcwd(); chdir( $dir );
		
		$rev_cur = exec( 'git rev-parse HEAD', $out, $ret );

		if ( 1 < strlen( $rev_prev ) && $rev_cur != $rev_prev )
		{	exec( 'git log --graph ' . $rev_prev . '..' . $rev_cur, $out, $ret );
			array_shift( $out );
			if ( count( $out ) ) $log = $rev_prev . '..' . $rev_cur . "\r\n" . implode( "\r\n", $out );
		} // end if
			
		chdir( $od );
		
		file_put_contents( $revfile, $rev_cur );

		return $log;	
	}

?>
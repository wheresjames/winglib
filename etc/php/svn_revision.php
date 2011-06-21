<?php

	function svn_revision_log( $dir )
	{
		$revfile = 'svnrev-' . md5( $dir ) . '.txt';
		$log = "";

		$rev_prev = 0;
		if ( is_file( $revfile ) )
			$rev_prev = (int)file_get_contents( $revfile );

		echo exec( 'svnversion ' . $dir );

		$rev_cur = (int)exec( 'svnversion' );

		if ( $rev_prev )
			$log = exec( 'svn log ' . $dir . ' -r ' . $rev_prev . ':' . $rev_cur );			

		file_put_contents( $revfile, $rev_cur );

		return $log;	
	}

?>
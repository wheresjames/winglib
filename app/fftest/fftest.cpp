    
    #include <string>
    
    #ifndef INT64_C
    #define INT64_C(c) (c ## LL)
    #define UINT64_C(c) (c ## ULL)
    #endif
    
    //#define _M
    #define _M printf( "%s(%d) : MARKER\n", __FILE__, __LINE__ )

	extern "C"
	{
		#include "libavcodec/avcodec.h"
		#include "libavformat/avformat.h"
	};
    
    
    AVFormatContext *fc = 0;
    int vi = -1, waitkey = 1;
    
    // < 0 = error
    // 0 = I-Frame
    // 1 = P-Frame
    // 2 = B-Frame
    // 3 = S-Frame
    int getVopType( const void *p, int len )
    {	
    	if ( !p || 6 >= len )
    		return -1;
    
    	unsigned char *b = (unsigned char*)p;
    	
    	// Verify NAL marker
    	if ( b[ 0 ] || b[ 1 ] || 0x01 != b[ 2 ] )
    	{	b++;
    		if ( b[ 0 ] || b[ 1 ] || 0x01 != b[ 2 ] )
    			return -1;
    	} // end if
    
    	b += 3;
    
    	// Verify VOP id
    	if ( 0xb6 == *b )
    	{	b++;
    		return ( *b & 0xc0 ) >> 6;
    	} // end if
    
    	switch( *b )
    	{	case 0x65 : return 0;
    		case 0x61 : return 1;
    		case 0x01 : return 2;
    	} // end switch
    
    	return -1;
    }
    
    void write_frame( const void* p, int len )
    {
    	if ( 0 > vi )
    		return;
    
    	AVStream *pst = fc->streams[ vi ];
    		
    	// Init packet
    	AVPacket pkt;
    	av_init_packet( &pkt );
    	pkt.flags |= ( 0 >= getVopType( p, len ) ) ? AV_PKT_FLAG_KEY : 0;	
    	pkt.stream_index = pst->index;
    	pkt.data = (uint8_t*)p;
    	pkt.size = len;
    	
    	// Wait for key frame
    	if ( waitkey )
    		if ( 0 == ( pkt.flags & AV_PKT_FLAG_KEY ) )
    			return;
    		else
    			waitkey = 0;

    	pkt.dts = AV_NOPTS_VALUE;
    	pkt.pts = AV_NOPTS_VALUE;
    
    //	av_write_frame( fc, &pkt );
    	av_interleaved_write_frame( fc, &pkt );
    }
    
    void destroy()
    {
    	waitkey = 1;
    	vi = -1;
    
    	if ( !fc )
    		return;
    
    _M;	av_write_trailer( fc );
    
    	if ( fc->oformat && !( fc->oformat->flags & AVFMT_NOFILE ) && fc->pb )
    		avio_close( fc->pb ); 
    			
    	// Free the stream
    _M;	av_free( fc );
    	
    	fc = 0;
    _M;	
    }
    
    int get_nal_type( void *p, int len )
    {
    	if ( !p || 5 >= len )
    		return -1;
    		
    	unsigned char *b = (unsigned char*)p;
    	
    	// Verify NAL marker
    	if ( b[ 0 ] || b[ 1 ] || 0x01 != b[ 2 ] )
    	{	b++;
    		if ( b[ 0 ] || b[ 1 ] || 0x01 != b[ 2 ] )
    			return -1;
    	} // end if
    
    	b += 3;

		return *b;
    }
    
    int create( void *p, int len )
    {
    	if ( 0x67 != get_nal_type( p, len ) )
    		return -1;

    	destroy();
    
    	const char *file = "test.avi";
    	CodecID codec_id = CODEC_ID_H264;
    //	CodecID codec_id = CODEC_ID_MPEG4;
    	int br = 1000000;
    	int w = 480;
    	int h = 354;
    	int fps = 15;
    
    	// Create container
    _M;	AVOutputFormat *of = av_guess_format( 0, file, 0 );
    	fc = avformat_alloc_context();
    	fc->oformat = of;
    	strcpy( fc->filename, file );
    
    	// Add video stream
    _M;	AVStream *pst = av_new_stream( fc, 0 );
    	vi = pst->index;
    
    	AVCodecContext *pcc = pst->codec;
    _M;	avcodec_get_context_defaults2( pcc, AVMEDIA_TYPE_VIDEO );
    	pcc->codec_type = AVMEDIA_TYPE_VIDEO;

    	pcc->codec_id = codec_id;
    	pcc->bit_rate = br;
    	pcc->width = w;
    	pcc->height = h;
    	pcc->time_base.num = 1;
    	pcc->time_base.den = fps;

    	// Init container
//    _M;	av_set_parameters( fc, 0 );
    	
    	if ( !( fc->oformat->flags & AVFMT_NOFILE ) )
    		avio_open( &fc->pb, fc->filename, AVIO_FLAG_WRITE );
    
    _M;	avformat_write_header( fc, 0 );
    
    _M;	return 1;
    }
    
    int main( int argc, char** argv )
    {
    	int f = 0, sz = 0;
    	char fname[ 256 ] = { 0 };
    	char buf[ 128 * 1024 ];

    	av_log_set_level( AV_LOG_ERROR );
    	av_register_all();
    
    	do
    	{
    		// Raw frames in v0.raw, v1.raw, v2.raw, ...
    //		sprintf( fname, "rawvideo/v%lu.raw", f++ );
    		sprintf( fname, "frames/frame%lu.bin", f++ );
    		printf( "%s\n", fname );
    		
    		FILE *fd = fopen( fname, "rb" );
    		if ( !fd )
    			sz = 0;
    		else
    		{
    			sz = fread( buf, 1, sizeof( buf ) - FF_INPUT_BUFFER_PADDING_SIZE, fd );
    			if ( 0 < sz )
    			{
    				memset( &buf[ sz ], 0, FF_INPUT_BUFFER_PADDING_SIZE );			

    				if ( !fc )
    					create( buf, sz );

    				if ( fc )
    					write_frame( buf, sz );
    				
    			} // end if
    			
    			fclose( fd );
    			
    		} // end else
    
    	} while ( 0 < sz );
    	
    	destroy();
    }

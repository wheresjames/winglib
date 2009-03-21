
# $1 = Existing file
# #2 = Link to create

for TOOL in c++filt addr2line readelf gcov size ar as ld nm ksymoops e2fs_install \
		    gprof strip libopt shellconfig libtoolize ranlib strings c++ g++ cpp \
		    gcc gdb ldd objcopy objdump initdconfig prelink-rtld prelink libindex \
		    makedepend dlltool windres gccbug
do
	[ ! -f $1-${TOOL} ] || ln -s $1-${TOOL} $2-${TOOL}
done


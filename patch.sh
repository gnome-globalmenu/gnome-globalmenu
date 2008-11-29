#! /bin/bash


function patch {
	edit=$1;
	shift;
	for i in $*; do
		echo -n patching $i with $edit ...
		if ! sed -e "$edit" $i > $i.new; then
			echo failed.
		else
			if cmp $i $i.new; then
				echo noting was done.
			else 
				echo OK.
			fi;
		fi
		mv $i.new $i; 
	done;
}


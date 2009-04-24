echo > LINGUAS
for i in launchpad/*.po; do
fn=${i/launchpad\/gnome-globalmenu-}
lang=${fn/.po}
	cp $i $fn
	echo $lang >> LINGUAS
done;

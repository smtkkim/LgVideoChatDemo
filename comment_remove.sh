find -iname *.cpp | xargs perl -i -wpe 'BEGIN{undef $/} s!/\*.*?\*/!!sg'
find -iname *.h | xargs perl -i -wpe 'BEGIN{undef $/} s!/\*.*?\*/!!sg'


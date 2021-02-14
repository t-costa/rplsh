set dimension with 1000
set inputsize with 200
set resources with 64

import "/home/costanzo/rplsh/examples/example_code.hpp"

annotate seq_vec_vec with servicetime 9
annotate map_vec_vec with servicetime 7
annotate map_minus with servicetime 12
annotate reduce_vec_double with servicetime 5
annotate dc_dummy with servicetime 7

m = map(map_vec_vec )
mf = map(farm( map_vec_vec ))
optimize mf with twotier
rewrite mf with compdel, rec
c = comp( m, mf)
dc = divide_conquer( dc_dummy )
cr = comp( map_minus , reduce_vec_double )
rcr = reduce(cr)
main = pipe( c, seq_vec_vec , dc, rcr)

#show main
#show main by servicetime , latency , compltime , pardegree , resources
#show main by servicetime , latency , compltime , pardegree , resources , unrank
show main by servicetime , latency , compltime , pardegree , resources , unrank, full
#show main by servicetime , compltime , resources , unrank, full

rewrite c with mapofcomp
show main by servicetime , latency , compltime , pardegree , resources , unrank, full
#potrei mettere sia rew c che rew main con rec

rewrite seq_vec_vec with farmintro
show main by servicetime , latency , compltime , pardegree , resources , unrank, full

optimize main with mapopt , farmopt , dcopt , reduceopt
show main by servicetime , latency , compltime , pardegree , resources , unrank, full
optimize main with mapopt , farmopt , dcopt , reduceopt , subexp
show main by servicetime , latency , compltime , pardegree , resources , unrank, full
optimize main with pipeopt
show main by servicetime , latency , compltime , pardegree , resources , unrank, full
optimize main with maxresources , subexp
show main by servicetime , latency , compltime , pardegree , resources , unrank, full

rewrite main with allrules
#show main by servicetime , latency , compltime , pardegree , resources , unrank, full

add source_vec in main
add drain_elem in main

#history
#gencode main[11] as example_code_ff in "examples/"

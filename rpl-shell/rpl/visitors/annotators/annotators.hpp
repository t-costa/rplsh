#ifndef rpl_annotators_hpp
#define rpl_annotators_hpp

#include "environment/rpl_environment.hpp"
#include "visitors/visitor_interface.hpp"

/**
 * Interface for annotators visitors
 */
struct ann_visitor : public skel_visitor
{
    explicit ann_visitor( rpl_environment& env );
    void visit(seq_node& n) override;
    void visit(source_node& n) override;
    void visit(drain_node& n) override;
    void visit(comp_node& n) override;
    void visit(pipe_node& n) override;
    void visit(farm_node& n) override;
    void visit(map_node& n) override;
    void visit(reduce_node& n) override;
    void visit(dc_node& n) override;
    void visit(id_node& n) override;

    /**
     * Implementations of this operator should return 'result';
     * result should be set to true or false in overridden
     * visit methods according to the logic of the annotator
     * @param n skeleton node
     * @param a annotation
     * @return true or false according to the annotator
     */
    virtual bool operator()( skel_node& n, ann_node& a ) = 0;

protected:
    rpl_environment& env;
    bool result;
};

struct ann_servicetime : public ann_visitor
{
    explicit ann_servicetime( rpl_environment& env );
    void visit(seq_node& n) override;
    void visit(source_node& n) override;
    void visit(drain_node& n) override;

    bool operator()( skel_node& n, ann_node& a ) override ;

private:
    double servicetime{};
};

struct ann_pardegree : public ann_visitor
{
    explicit ann_pardegree( rpl_environment& env );
    void visit(farm_node& n) override;
    void visit(map_node& n) override;
    void visit(reduce_node& n) override;
    void visit(dc_node& n) override;

    bool operator()( skel_node& n, ann_node& a ) override ;

private:
    std::size_t nw;
};

struct ann_datap : public ann_visitor
{
    explicit ann_datap( rpl_environment& env );
    void visit(seq_node& n) override;

    bool operator()( skel_node& n, ann_node& a ) override ;

private:
    bool datap;
};

struct ann_typein : public ann_visitor
{
    explicit ann_typein( rpl_environment& env );
    void visit( seq_node& n ) override;
    void visit( drain_node& n ) override;
    bool operator()( skel_node& n, ann_node& a ) override ;

private:
    std::string type;
};

struct ann_typeout : public ann_visitor
{
    explicit ann_typeout( rpl_environment& env );
    void visit( seq_node& n ) override;
    void visit( source_node& n ) override;
    bool operator()( skel_node& n, ann_node& a ) override ;

private:
    std::string type;
};

struct ann_grain : public ann_visitor
{
    explicit ann_grain(rpl_environment& env);
    void visit (map_node& n) override;
    void visit (reduce_node& n) override;

    bool operator() (skel_node& n, ann_node& a) override;

private:
    long value;
    std::string scheduling_type;
};

struct ann_step : public ann_visitor
{
    explicit ann_step(rpl_environment& env);
    void visit (map_node& n) override;
    void visit (reduce_node& n) override;

    bool operator() (skel_node& n, ann_node& a) override;

private:
    long value;
};

struct ann_dc_capable : public ann_visitor
{
    explicit ann_dc_capable(rpl_environment& env);
    void visit (seq_node& n) override;

    bool operator() (skel_node& n, ann_node& a) override;

private:
    bool dc_flag;
};

struct ann_cutoff : public ann_visitor {
    explicit ann_cutoff(rpl_environment& env);
    void visit (dc_node& n) override;

    bool operator() (skel_node& n, ann_node& a) override;

private:
    long value;
};

struct ann_schedule : public ann_visitor {
    explicit ann_schedule(rpl_environment& env);
    void visit(dc_node& n) override;

    bool operator() (skel_node& n, ann_node& a) override;

private:
    long value;
    std::string scheduling_type;
};
#endif

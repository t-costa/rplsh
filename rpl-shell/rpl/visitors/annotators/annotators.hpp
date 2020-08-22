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

struct ann_inside_data_parallel : public ann_visitor
{
    explicit ann_inside_data_parallel( rpl_environment& env);
    void visit(seq_node& n) override;
    void visit(comp_node& n) override;
    void visit(id_node& n) override;

    bool operator()( skel_node& n, ann_node& a) override;

private:
    bool inside_data_parallel;
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

#endif

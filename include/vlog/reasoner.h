#ifndef REASONER_H
#define REASONER_H

#include <vlog/concepts.h>
#include <vlog/edb.h>
#include <vlog/fctable.h>
#include <vlog/seminaiver.h>

#include <trident/kb/kb.h>
#include <trident/kb/querier.h>

#include <trident/sparql/query.h>

#define QUERY_MAT 0
#define QUERY_ONDEM 1

typedef enum {TOPDOWN, MAGIC} ReasoningMode;

class Reasoner {
private:

    const uint64_t threshold;

    void cleanBindings(std::vector<Term_t> &bindings, std::vector<uint8_t> * posJoins,
                       TupleTable *input);

    /*TupleTable *getVerifiedBindings(QSQQuery &query,
                                    std::vector<uint8_t> * posJoins,
                                    std::vector<Term_t> *possibleValuesJoins,
                                    EDBLayer &layer, Program &program, DictMgmt *dict,
                                    bool returnOnlyVars);*/

    FCBlock getBlockFromQuery(Literal constantsQuery, Literal &boundQuery, std::vector<uint8_t> *posJoins,
                              std::vector<Term_t> *possibleValuesJoins);
public:

    Reasoner(const uint64_t threshold) : threshold(threshold) {}

    ReasoningMode chooseMostEfficientAlgo(Literal &query,
                                          EDBLayer &layer, Program &program,
                                          std::vector<uint8_t> *posBindings,
                                          std::vector<Term_t> *valueBindings);

    static TupleIterator *getTopDownIterator(Literal &query,
                                      std::vector<uint8_t> * posJoins,
                                      std::vector<Term_t> *possibleValuesJoins,
                                      EDBLayer &layer, Program &program,
                                      bool returnOnlyVars);

    TupleIterator *getMagicIterator(Literal &query,
                                    std::vector<uint8_t> * posJoins,
                                    std::vector<Term_t> *possibleValuesJoins,
                                    EDBLayer &layer, Program &program,
                                    bool returnOnlyVars);

    /*
    TupleIterator *getIncrReasoningIterator(Pattern *pattern,
                                            std::vector<uint8_t> * posJoins,
                                            std::vector<Term_t> *possibleValuesJoins,
                                            EDBLayer &layer, Program &program,
                                            DictMgmt *dict,
                                            bool returnOnlyVars);
    */

    size_t estimate(Literal &query, std::vector<uint8_t> *posBindings,
                  std::vector<Term_t> *valueBindings, EDBLayer &layer,
                  Program &);

    static std::shared_ptr<SemiNaiver> fullMaterialization(EDBLayer &layer,
            Program *p, bool opt_intersect, bool opt_filtering);

    static std::shared_ptr<SemiNaiver> getSemiNaiver(EDBLayer &layer,
        Program *p, bool opt_intersect, bool opt_filtering);

    //static int materializationOrOnDemand(const uint64_t matThreshold, std::vector<std::shared_ptr<SPARQLOperator>> &patterns);

    ~Reasoner() {
    }
};
#endif
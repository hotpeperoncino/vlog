#ifndef _SEMI_NAIVER_THREADED_H
#define _SEMI_NAIVER_THREADED_H

#include <vlog/seminaiver.h>

#include <boost/thread/mutex.hpp>
#include <boost/thread/tss.hpp>

struct SemiNaiver_Threadlocal {
    std::vector<FCBlock> listDerivations;
    std::vector<StatsRule> statsRuleExecution;
    size_t iteration;
};

class StatusRuleExecution_ThreadSafe {
private:
    //Internal data structures

    //Hold all rules to execution
    boost::mutex mutexRules;
    int rulecount;
    const int nrules;

    std::vector<ResultJoinProcessor*> tmpderivations;

public:

    StatusRuleExecution_ThreadSafe(const int nrules);

    int getRuleIDToExecute();

    void registerDerivations(ResultJoinProcessor *res);

    std::vector<ResultJoinProcessor*> &getTmpDerivations() {
        return  tmpderivations;
    }
};

class SemiNaiverThreaded: public SemiNaiver {

private:
    //const int nthreads;
    boost::thread_specific_ptr<SemiNaiver_Threadlocal> thread_data;

    bool marked[MAX_NPREDS];
    bool newMarked[MAX_NPREDS];

    /*** VARIOUS MUTEXES */
    boost::mutex mutexInsert;
    boost::mutex mutexIteration;
    boost::mutex mutexGetTable;
    boost::mutex mutexStatistics;
    boost::mutex mutexListDer;
    //boost::mutex mutexGetIterator;
    const int interRuleThreads;

    //Create one mutex per table
    boost::shared_mutex mutexes[MAX_NPREDS];

    size_t getAtomicIteration() {
        boost::mutex::scoped_lock lock(mutexIteration);
        return iteration++;
    }

    bool doGlobalConsolidation(StatusRuleExecution_ThreadSafe &data);

    bool tryLock(std::vector<PredId_t> &predicates, PredId_t idHeadPredicate);

    void lock(std::vector<PredId_t> &predicates, PredId_t idHeadPredicate);

    void unlock(std::vector<PredId_t> &predicates, PredId_t idHeadPredicate);

public:
    SemiNaiverThreaded(std::vector<Rule> ruleset,
                       EDBLayer &layer,
                       Program *program,
                       bool opt_intersect,
                       bool opt_filtering,
                       bool shuffleRules,
                       const int nthreads,
		       const int interRuleThreads) : SemiNaiver(ruleset, layer,
                                   program, opt_intersect, opt_filtering, true,
                                   nthreads, shuffleRules),
        interRuleThreads(interRuleThreads) {

	// Marks for parallel version
	for (int i = 0; i < MAX_NPREDS; i++) {
	    marked[i] = true;
	    newMarked[i] = false;
	}
    }

protected:
    long getNLastDerivationsFromList();

    void saveDerivationIntoDerivationList(FCTable *endTable);

    void saveStatistics(StatsRule &stats);

    FCTable *getTable(const PredId_t pred, const uint8_t card);

    FCIterator getTableFromEDBLayer(const Literal & literal);

    void runThread(StatusRuleExecution_ThreadSafe *status,
                   std::vector<StatIteration> *costRules,
                   size_t lastExec);

    void executeUntilSaturation(std::vector<StatIteration> &costRules);
};

#endif

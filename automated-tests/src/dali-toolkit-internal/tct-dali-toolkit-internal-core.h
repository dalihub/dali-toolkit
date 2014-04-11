#ifndef __TCT_DALI_TOOLKIT_INTERNAL_CORE_H__
#define __TCT_DALI_TOOLKIT_INTERNAL_CORE_H__

#include "testcase.h"

extern void dali_text_view_helper_and_debug_startup(void);
extern void dali_text_view_helper_and_debug_cleanup(void);
extern void dali_text_view_processor_types_startup(void);
extern void dali_text_view_processor_types_cleanup(void);
extern void dali_text_view_processor_startup(void);
extern void dali_text_view_processor_cleanup(void);
extern void dali_text_view_relayout_utils_startup(void);
extern void dali_text_view_relayout_utils_cleanup(void);
extern void dali_text_view_startup(void);
extern void dali_text_view_cleanup(void);

extern int UtcDaliPushButtonInterruptEventWhenInsensitive(void);
extern int UtcDaliPushButtonInterruptEventWhenNonVisible(void);
extern int UtcDaliPushButtonProperties(void);
extern int UtcDaliTextViewGetIndicesFromGlobalCharacterIndex(void);
extern int UtcDaliTextViewDebugCouts(void);
extern int UtcDaliTextViewDefaultConstructorDestructor_PT(void);
extern int UtcDaliTextViewCopyConstructorOperator(void);
extern int UtcDaliTextViewEqualityOperator(void);
extern int UtcDaliTextViewSplitInLines(void);
extern int UtcDaliTextViewSplitInWords(void);
extern int UtcDaliTextViewBeginsRightToLeftCharacter(void);
extern int UtcDaliTextViewContainsRightToLeftCharacter(void);
extern int UtcDaliTextViewFindNearestWord(void);
extern int UtcDaliTextViewDefaultConstructorDestructor_RU(void);
extern int UtcDaliTextViewCalculateSubLineLayout(void);
extern int UtcDaliTextViewCalculateAlignmentOffsets(void);
extern int UtcDaliTextViewCalculateJustificationOffsets(void);
extern int UtcDaliTextViewCalculateVisibility(void);
extern int UtcDaliTextViewMiscelaneousAsserts(void);
extern int UtcDaliTextViewCreateTextInfo(void);
extern int UtcDaliTextViewSplitWord(void);
extern int UtcDaliTextViewUpdateTextInfo(void);
extern int UtcDaliTextViewSplitWordGroup(void);
extern int UtcDaliTextViewSplitLine(void);
extern int UtcDaliTextViewMergeWord01(void);
extern int UtcDaliTextViewMergeWord02(void);
extern int UtcDaliTextViewMergeGroup01(void);
extern int UtcDaliTextViewMergeGroup02(void);
extern int UtcDaliTextViewMergeLine01(void);
extern int UtcDaliTextViewMergeLine02(void);
extern int UtcDaliTextViewRemoveCharactersFromWord(void);
extern int UtcDaliTextViewRemoveWordsFromGroup(void);
extern int UtcDaliTextViewRemoveGroupsFromLine(void);

testcase tc_array[] = {
    {"UtcDaliPushButtonInterruptEventWhenInsensitive", UtcDaliPushButtonInterruptEventWhenInsensitive, NULL, NULL},
    {"UtcDaliPushButtonInterruptEventWhenNonVisible", UtcDaliPushButtonInterruptEventWhenNonVisible, NULL, NULL},
    {"UtcDaliPushButtonProperties", UtcDaliPushButtonProperties, NULL, NULL},
    {"UtcDaliTextViewGetIndicesFromGlobalCharacterIndex", UtcDaliTextViewGetIndicesFromGlobalCharacterIndex, dali_text_view_helper_and_debug_startup, dali_text_view_helper_and_debug_cleanup},
    {"UtcDaliTextViewDebugCouts", UtcDaliTextViewDebugCouts, dali_text_view_helper_and_debug_startup, dali_text_view_helper_and_debug_cleanup},
    {"UtcDaliTextViewDefaultConstructorDestructor_PT", UtcDaliTextViewDefaultConstructorDestructor_PT, dali_text_view_processor_types_startup, dali_text_view_processor_types_cleanup},
    {"UtcDaliTextViewCopyConstructorOperator", UtcDaliTextViewCopyConstructorOperator, dali_text_view_processor_types_startup, dali_text_view_processor_types_cleanup},
    {"UtcDaliTextViewEqualityOperator", UtcDaliTextViewEqualityOperator, dali_text_view_processor_types_startup, dali_text_view_processor_types_cleanup},
    {"UtcDaliTextViewSplitInLines", UtcDaliTextViewSplitInLines, dali_text_view_processor_startup, dali_text_view_processor_cleanup},
    {"UtcDaliTextViewSplitInWords", UtcDaliTextViewSplitInWords, dali_text_view_processor_startup, dali_text_view_processor_cleanup},
    {"UtcDaliTextViewBeginsRightToLeftCharacter", UtcDaliTextViewBeginsRightToLeftCharacter, dali_text_view_processor_startup, dali_text_view_processor_cleanup},
    {"UtcDaliTextViewContainsRightToLeftCharacter", UtcDaliTextViewContainsRightToLeftCharacter, dali_text_view_processor_startup, dali_text_view_processor_cleanup},
    {"UtcDaliTextViewFindNearestWord", UtcDaliTextViewFindNearestWord, dali_text_view_processor_startup, dali_text_view_processor_cleanup},
    {"UtcDaliTextViewDefaultConstructorDestructor_RU", UtcDaliTextViewDefaultConstructorDestructor_RU, dali_text_view_relayout_utils_startup, dali_text_view_relayout_utils_cleanup},
    {"UtcDaliTextViewCalculateSubLineLayout", UtcDaliTextViewCalculateSubLineLayout, dali_text_view_relayout_utils_startup, dali_text_view_relayout_utils_cleanup},
    {"UtcDaliTextViewCalculateAlignmentOffsets", UtcDaliTextViewCalculateAlignmentOffsets, dali_text_view_relayout_utils_startup, dali_text_view_relayout_utils_cleanup},
    {"UtcDaliTextViewCalculateJustificationOffsets", UtcDaliTextViewCalculateJustificationOffsets, dali_text_view_relayout_utils_startup, dali_text_view_relayout_utils_cleanup},
    {"UtcDaliTextViewCalculateVisibility", UtcDaliTextViewCalculateVisibility, dali_text_view_relayout_utils_startup, dali_text_view_relayout_utils_cleanup},
    {"UtcDaliTextViewMiscelaneousAsserts", UtcDaliTextViewMiscelaneousAsserts, dali_text_view_relayout_utils_startup, dali_text_view_relayout_utils_cleanup},
    {"UtcDaliTextViewCreateTextInfo", UtcDaliTextViewCreateTextInfo, dali_text_view_startup, dali_text_view_cleanup},
    {"UtcDaliTextViewSplitWord", UtcDaliTextViewSplitWord, dali_text_view_startup, dali_text_view_cleanup},
    {"UtcDaliTextViewUpdateTextInfo", UtcDaliTextViewUpdateTextInfo, dali_text_view_startup, dali_text_view_cleanup},
    {"UtcDaliTextViewSplitWordGroup", UtcDaliTextViewSplitWordGroup, dali_text_view_startup, dali_text_view_cleanup},
    {"UtcDaliTextViewSplitLine", UtcDaliTextViewSplitLine, dali_text_view_startup, dali_text_view_cleanup},
    {"UtcDaliTextViewMergeWord01", UtcDaliTextViewMergeWord01, dali_text_view_startup, dali_text_view_cleanup},
    {"UtcDaliTextViewMergeWord02", UtcDaliTextViewMergeWord02, dali_text_view_startup, dali_text_view_cleanup},
    {"UtcDaliTextViewMergeGroup01", UtcDaliTextViewMergeGroup01, dali_text_view_startup, dali_text_view_cleanup},
    {"UtcDaliTextViewMergeGroup02", UtcDaliTextViewMergeGroup02, dali_text_view_startup, dali_text_view_cleanup},
    {"UtcDaliTextViewMergeLine01", UtcDaliTextViewMergeLine01, dali_text_view_startup, dali_text_view_cleanup},
    {"UtcDaliTextViewMergeLine02", UtcDaliTextViewMergeLine02, dali_text_view_startup, dali_text_view_cleanup},
    {"UtcDaliTextViewRemoveCharactersFromWord", UtcDaliTextViewRemoveCharactersFromWord, dali_text_view_startup, dali_text_view_cleanup},
    {"UtcDaliTextViewRemoveWordsFromGroup", UtcDaliTextViewRemoveWordsFromGroup, dali_text_view_startup, dali_text_view_cleanup},
    {"UtcDaliTextViewRemoveGroupsFromLine", UtcDaliTextViewRemoveGroupsFromLine, dali_text_view_startup, dali_text_view_cleanup},
    {NULL, NULL}
};

#endif // __TCT_DALI_TOOLKIT_INTERNAL_CORE_H__

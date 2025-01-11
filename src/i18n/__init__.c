#include <stdlib.h>
#include "utils/defines.h"
#include "__init__.h"

#include <string.h>

const iwr_i18n_str_t* T = NULL;
static const iwr_i18n_t* s_i18n = NULL;

static const iwr_i18n_t* s_locales[] = {
#define I18N_EXPAND_LOCALE_AS_ITEM(e) &iwr_i18n_##e,
    IWR_I18N_LOCALE_TABLE(I18N_EXPAND_LOCALE_AS_ITEM)
#undef I18N_EXPAND_LOCALE_AS_ITEM
};

void iwr_i18n_init(void)
{
#define I18N_EXPAND_AS_CHECK(s) \
    if (i18n->strings->s == NULL) {\
        i18n->strings->s = iwr_i18n_en_US.strings->s;\
    }

    for (size_t i = 0; i < ARRAY_SIZE(s_locales); i++)
    {
        const iwr_i18n_t* i18n = s_locales[i];
        IWR_I18N_STRING_TABLE(I18N_EXPAND_AS_CHECK)
    }

    iwr_i18n_setlocale("en_US");

#undef I18N_EXPAND_AS_CHECK
}

void iwr_i18n_exit(void)
{
}

void iwr_i18n_setlocale(const char* locale)
{
    for (size_t i = 0; i < ARRAY_SIZE(s_locales); i++)
    {
        const iwr_i18n_t* i18n = s_locales[i];
        if (strcmp(i18n->locale_name, locale) == 0)
        {
            T = i18n->strings;
            s_i18n = i18n;
            return;
        }
    }

    /* If not found, set to en_US. */
    T = iwr_i18n_en_US.strings;
    s_i18n = &iwr_i18n_en_US;
}

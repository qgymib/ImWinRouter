#ifndef I18_INIT_H
#define I18_INIT_H

/**
 * @brief i18n strings.
 */
// clang-format off
#define IWR_I18N_STRING_TABLE(xx)    \
    xx(about)           \
    xx(debug)           \
    xx(generic)         \
    xx(help)            \
    xx(homepage)        \
    xx(localization)    \
    xx(name)            \
    xx(notification)    \
    xx(preferences)     \
    xx(refresh)         \
    xx(router)          \
    xx(settings)        \
    xx(tools)           \
    xx(translations)    \
    xx(version)
// clang-format on

/**
 *@brief i18n locale
 */
// clang-format off
#define IWR_I18N_LOCALE_TABLE(xx)    \
    xx(en_US)    \
    xx(zh_CN)
// clang-format on

/**
 * @brief Declare locale.
 * @param[in] name Locale name. See #IWR_I18N_LOCALE_TABLE.
 */
// clang-format off
#define IWR_I18N_LOCALE_DECLARE(name)   \
    static iwr_i18n_str_t s_iwr_i18n_strings_## name;\
    const iwr_i18n_t iwr_i18n_## name = {\
        I18N_LOCALE_##name, #name, &s_iwr_i18n_strings_##name,\
    };\
    static iwr_i18n_str_t s_iwr_i18n_strings_## name
// clang-format on

#ifdef __cplusplus
extern "C" {
#endif

typedef enum iwr_i18n_strid
{
#define I18N_EXPAND_AS_ENUM(a) I18N_STR_##a,
    IWR_I18N_STRING_TABLE(I18N_EXPAND_AS_ENUM)
#undef I18N_EXPAND_AS_ENUM
} iwr_i18n_strid_t;

typedef enum iwr_i18n_locale
{
#define I18N_EXPAND_LOCALE_AS_ENUM(a) I18N_LOCALE_##a,
    IWR_I18N_LOCALE_TABLE(I18N_EXPAND_LOCALE_AS_ENUM)
#undef I18N_EXPAND_LOCALE_AS_ENUM
} iwr_i18n_locale_t;

typedef struct iwr_i18n_str
{
#define I18N_EXPAND_STRING_AS_FIELD(a) const char* a;
    IWR_I18N_STRING_TABLE(I18N_EXPAND_STRING_AS_FIELD)
#undef I18N_EXPAND_STRING_AS_FIELD
} iwr_i18n_str_t;

typedef struct iwr_i18n
{
    iwr_i18n_locale_t locale;
    const char*       locale_name;
    iwr_i18n_str_t*   strings;
} iwr_i18n_t;

/**
 * @brief Declare global available locales.
 * @{
 */
#define I18N_EXPAND_LOCALE_AS_EXTERN(e) extern const iwr_i18n_t iwr_i18n_##e;
IWR_I18N_LOCALE_TABLE(I18N_EXPAND_LOCALE_AS_EXTERN)
#undef I18N_EXPAND_LOCALE_AS_EXTERN
/**
 * @}
 */

/**
 * @brief Global translation.
 */
extern const iwr_i18n_str_t* T;

/**
 * @brief Initialize i18n.
 */
void iwr_i18n_init(void);

/**
 * @brief Cleanup i18n.
 */
void iwr_i18n_exit(void);

/**
 * @brief Set global locale.
 * @param[in] locale Locale.
 */
void iwr_i18n_setlocale(const char* locale);

#ifdef __cplusplus
}
#endif
#endif // I18_INIT_H

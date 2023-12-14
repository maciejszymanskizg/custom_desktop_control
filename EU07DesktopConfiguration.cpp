#include "EU07DesktopConfiguration.h"
#include "EU07ConfigurationIDs.h"
#include "Logger.h"

#define EU07_CONFIG_ENTRY(_id, _name, _min, _max, _init) \
	{ \
		.id = _id, \
		.name = _name, \
		.min_value = _min, \
		.max_value = _max, \
		.init_value = _init, \
	}

struct EU07_Configuration_Entry EU07_config[] = {
	EU07_CONFIG_ENTRY(EU07_ID_HASLER_PREDKOSC, "Predkowsc (Hasler)", 0, 150, 0),
	EU07_CONFIG_ENTRY(EU07_ID_AMPEROMIERZ_NN, "Amperomierz NN", 0, 60, 0),
	EU07_CONFIG_ENTRY(EU07_ID_WOLTOMIERZ_NN, "Woltomierz NN", 0, 150, 0),
	EU07_CONFIG_ENTRY(EU07_ID_AMPEROMIERZ_WN1, "Amperomierz WN1", 0, 800, 0),
	EU07_CONFIG_ENTRY(EU07_ID_AMPEROMIERZ_WN2, "Amperomierz WN2", 0, 800, 0),
	EU07_CONFIG_ENTRY(EU07_ID_WOLTOMIERZ_WN, "Woltomierz NN", 0, 4000, 0),
	EU07_CONFIG_ENTRY(EU07_ID_MANOMETR_CYLINDER_HAMULCOWY, "Manometr cylindrow hamulcowych", 0, 1000, 0),
	EU07_CONFIG_ENTRY(EU07_ID_MANOMETR_PRZEWOD_GLOWNY, "Manometr przewodu glownego", 0, 1000, 0),
	EU07_CONFIG_ENTRY(EU07_ID_MANOMETR_ZBIORNIK_GLOWNY, "Manometr zbiorika glownego", 0, 1600, 0),
	EU07_CONFIG_ENTRY(EU07_ID_ZAWOR_HAMULCA_GLOWNEGO, "Zawor hamulca glownego", 0, 1023, 0),
	EU07_CONFIG_ENTRY(EU07_ID_ZAWOR_HAMULCA_DODATKOWEGO, "Zawor hamulca dodatkowego", 0, 1023, 0),
	EU07_CONFIG_ENTRY(EU07_ID_NASTAWNIK_KIERUNKU_JAZDY, "Nastawnik kierunku jazdy", 0, 2, 0),
	EU07_CONFIG_ENTRY(EU07_ID_NASTAWNIK_JAZDY, "Nastawnik jazdy", 0, 43, 0),
	EU07_CONFIG_ENTRY(EU07_ID_NASTAWNIK_BOCZNIKOWANIA, "Nastawnik bocznikowania", 0, 6, 0),
	EU07_CONFIG_ENTRY(EU07_ID_WLACZNIK_ROZRZADU, "Wlacznik rozrzadu", 0, 1, 0),
	EU07_CONFIG_ENTRY(EU07_ID_WLACZNIK_BATERII, "Wlacznik baterii", 0, 1, 0),
	EU07_CONFIG_ENTRY(EU07_ID_WLACZNIK_OGRZEWANIA_NOG, "Wlacznik ogrzewania nog", 0, 1, 0),
	EU07_CONFIG_ENTRY(EU07_ID_WLACZNIK_PRZYCIEMNIENIA_KABINY, "Wlacznik przyciemnienia kabiny", 0, 1, 0),
	EU07_CONFIG_ENTRY(EU07_ID_WLACZNIK_PRZYCIEMNIENIA_PP, "Wlacznik przyciemnienia przyrzadow pomiarowych", 0, 1, 0),
	EU07_CONFIG_ENTRY(EU07_ID_WLACZNIK_PRZYCIEMNIENIA_LS1, "Wlacznik przyciemnienia lampek sygnalizacyjnych 1", 0, 1, 0),
	EU07_CONFIG_ENTRY(EU07_ID_WLACZNIK_PRZYCIEMNIENIA_LS2, "Wlacznik przyciemnienia lampek sygnalizacyjnych 2", 0, 1, 0),
	EU07_CONFIG_ENTRY(EU07_ID_WLACZNIK_SYGNAL_CZERWONY_LEWY, "Wlacznik sygnalu czerwonego (lewy)", 0, 1, 0),
	EU07_CONFIG_ENTRY(EU07_ID_WLACZNIK_REZERWA, "Wlacznik rezerwy", 0, 1, 0),
	EU07_CONFIG_ENTRY(EU07_ID_WLACZNIK_SYGNAL_CZERWONY_PRAWY, "Wlacznik sygnalu czerwonego (prawy)", 0, 1, 0),
	EU07_CONFIG_ENTRY(EU07_ID_WLACZNIK_OSWIETLENIE_OGOLNE, "Wlacznik oswietlenia ogolnego", 0, 1, 0),
	EU07_CONFIG_ENTRY(EU07_ID_WLACZNIK_OSWIETLENIE_KABINY, "Wlacznik oswietlenia kabiny", 0, 1, 0),
	EU07_CONFIG_ENTRY(EU07_ID_WLACZNIK_OSWIETLENIE_PP, "Wlacznik oswietlenia przyrzadow pomiarowych", 0, 1, 0),
	EU07_CONFIG_ENTRY(EU07_ID_WLACZNIK_OSWIETLENIE_SZAFY_WN, "Wlacznik oswietlenia szafy WN", 0, 1, 0),
	EU07_CONFIG_ENTRY(EU07_ID_WLACZNIK_REFLEKTOR_LEWY, "Wlacznik reflektora lewego", 0, 1, 0),
	EU07_CONFIG_ENTRY(EU07_ID_WLACZNIK_REFLEKTOR_GORNY, "Wlacznik reflekotra gornego", 0, 1, 0),
	EU07_CONFIG_ENTRY(EU07_ID_WLACZNIK_REFLEKTOR_PRAWY, "Wlacznik reflektora prawego", 0, 1, 0),
	EU07_CONFIG_ENTRY(EU07_ID_WLACZNIK_PRZYCIEMNIENIE_SHP, "Wlacznik przyciemnienia SHP", 0, 1, 0),
	EU07_CONFIG_ENTRY(EU07_ID_WLACZNIK_PANTOGRAF1, "Wlacznik pantografu (przod)", 0, 1, 0),
	EU07_CONFIG_ENTRY(EU07_ID_WLACZNIK_SPREZARKA, "Wlacznik sprezarki", 0, 1, 0),
	EU07_CONFIG_ENTRY(EU07_ID_WLACZNIK_PRZETWORNICA, "Wlacznik przetwornicy", 0, 1, 0),
	EU07_CONFIG_ENTRY(EU07_ID_WLACZNIK_PRZYCIEMNIENIE_CZUWAKA, "Wlacznik przyciemnienia czuwaka", 0, 1, 0),
	EU07_CONFIG_ENTRY(EU07_ID_WLACZNIK_PANTOGRAF2, "Wlacznik pantografu (tyl)", 0, 1, 0),
	EU07_CONFIG_ENTRY(EU07_ID_WLACZNIK_OGRZEWANIE_POCIAGU, "Wlacznik ogrzewania pociagu", 0, 1, 0),
	EU07_CONFIG_ENTRY(EU07_ID_WLACZNIK_PRZYCIEMNIENIE_REFL1, "Wlacznik przyciemnienia reflektorow 1", 0, 1, 0),
	EU07_CONFIG_ENTRY(EU07_ID_WLACZNIK_PRZYCIEMNIENIE_REFL2, "Wlacznik przyciemnienia reflektorow 2", 0, 1, 0),
	EU07_CONFIG_ENTRY(EU07_ID_PRZELACZNIK_RODZAJU_HAMULCA, "Przelacznik rodzaju hamulca", 0, 2, 0),
	EU07_CONFIG_ENTRY(EU07_ID_PRZELACZNIK_SILY_NACISKU, "Przelacznik dostosowania sil do nacisku kol", 0, 1, 0),
	EU07_CONFIG_ENTRY(EU07_ID_PRZELACZNIK_PRADU_ROZRUCHU, "Przelacznik pradu rozruchu", 0, 1, 0),
	EU07_CONFIG_ENTRY(EU07_ID_PRZELACZNIK_WYCIERACZKI, "Przelacznik wycieraczki", 0, 3, 0),
	EU07_CONFIG_ENTRY(EU07_ID_PRZYCISK_REZERWA, "Przycisk rezerwy", 0, 1, 0),
	EU07_CONFIG_ENTRY(EU07_ID_PRZYCISK_ODBLOK_PRZEK_NAD_SPREZ, "Przycisk odblokowania przekaznikow nadmiarowych sprezarki", 0, 1, 0),
	EU07_CONFIG_ENTRY(EU07_ID_PRZYCISK_WYLACZNIK_SZYBKI_WYL, "Przycisk wylacznik szybki wylaczony", 0, 1, 0),
	EU07_CONFIG_ENTRY(EU07_ID_PRZYCISK_WYLACZNIK_SZYBKI_ZAL, "Przycisk wylacznik szybki zalaczony", 0, 1, 0),
	EU07_CONFIG_ENTRY(EU07_ID_PRZYCISK_ODBL_PRZEK_NADM_SIL_TRAK, "Przycisk odblokowania przekaznikow nadmiarowych silnikow trakcyjnych", 0, 1, 0),
	EU07_CONFIG_ENTRY(EU07_ID_PRZYCISK_ODBL_PRZEK_NADM_PRZETW_OGRZ, "Przycisk odblokowania przekaznikow nadmiarowych przetwornicy ogrzewania", 0, 1, 0),
	EU07_CONFIG_ENTRY(EU07_ID_PRZYCISK_STYCZNIKI_LINIOWE_WYL, "Przycisk styczniki liniowe wylaczone", 0, 1, 0),
	EU07_CONFIG_ENTRY(EU07_ID_PRZYCISK_POSLIZG, "Przycisk poslizg", 0, 1 ,0),
	EU07_CONFIG_ENTRY(EU07_ID_PRZYCISK_LUZOWANIE_HAMULCA, "Przycisk luzowania hamulca", 0, 1, 0),
	EU07_CONFIG_ENTRY(EU07_ID_PRZYCISK_SHP, "Przycisk kasowania czuwaka i SHP", 0, 1, 0),
	EU07_CONFIG_ENTRY(EU07_ID_LAMPKA_SHP, "Lampka SHP", 0, 1, 0),
	EU07_CONFIG_ENTRY(EU07_ID_LAMPKA_CZUWAK, "Lampka czuwaka", 0, 1, 0),
	EU07_CONFIG_ENTRY(EU07_ID_LAMPKA_PRZEK_NADMIAR_SPREZAREK, "Lampka przekaznikow nadmiarowych sprezarek", 0, 1, 0),
	EU07_CONFIG_ENTRY(EU07_ID_LAMPKA_PRZEK_NADMIAR_WENTYL, "Lampka przekaznikow nadmiarowych wentylatorow", 0, 1, 0),
	EU07_CONFIG_ENTRY(EU07_ID_LAMPKA_WYLACZNIK_SZYBKI, "Lampka wylacznika szybkiego", 0, 1, 0),
	EU07_CONFIG_ENTRY(EU07_ID_LAMPKA_PRZEK_NADMIAR_SIL_TRAK, "Lampka przekaznikow nadmiarowych silnikow trakcyjnych", 0, 1, 0),
	EU07_CONFIG_ENTRY(EU07_ID_LAMPKA_PRZEK_ROZNICOWY, "Lampka przekaznika roznicowego", 0, 1, 0),
	EU07_CONFIG_ENTRY(EU07_ID_LAMPKA_PRZEK_NADM_PRZETW_OGRZEW, "Lampka przekaznika nadmiarowego przetwornicy ogrzewania", 0, 1, 0),
	EU07_CONFIG_ENTRY(EU07_ID_LAMPKA_STYCZNIKI_LINIOWE, "Lampka stycznikow liniowych", 0, 1, 0),
	EU07_CONFIG_ENTRY(EU07_ID_LAMPKA_POSLIZG, "Lampka poslizgu", 0, 1, 0),
};

EU07_Desktop_Configuration::EU07_Desktop_Configuration() : Configuration("EU07 Configuration")
{
	unsigned int i = 0;
	for (i = 0; i < sizeof(EU07_config) / sizeof(struct EU07_Configuration_Entry); i++) {
		ConfigurationEntry *e = new ConfigurationEntry(EU07_config[i].name, EU07_config[i].min_value,
				EU07_config[i].max_value, EU07_config[i].init_value);

		if (e != nullptr)
			addConfigurationEntry(EU07_config[i].id, e);
	}
}

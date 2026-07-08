#pragma once
// ============================================================================
//  Strings.h — Multilingual UI strings
//
//  The language is AUTO-detected from the Windows UI language (GetUserDefaultUILanguage).
//  Supported languages: English (DEFAULT), Turkish, German, French, Spanish,
//  Italian, Portuguese, Russian, Japanese, Chinese, Korean, Dutch, Polish,
//  Ukrainian. For unlisted languages, English is used.
//
//  ARCHITECTURE NOTE (mechanism preserved): Each string is a "Txt" table that
//  implicitly converts to const wchar_t* / std::wstring; so existing str::XXX
//  usages work UNCHANGED. A formatter for std::format is also defined.
//
//  NOTE: geri_al.cmd and assistant.log are INTENTIONALLY not translated — the
//  revert script's entry matching depends on these strings (mechanism safety).
// ============================================================================
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>
#include <string_view>
#include <format>

namespace str {

// Language detection
enum Lang { L_TR = 0, L_EN, L_DE, L_FR, L_ES, L_IT, L_PT, L_RU,
            L_JA, L_ZH, L_KO, L_NL, L_PL, L_UK, L_COUNT };

inline Lang detectLang() {
    switch (PRIMARYLANGID(GetUserDefaultUILanguage())) {
        case LANG_TURKISH:    return L_TR;
        case LANG_GERMAN:     return L_DE;
        case LANG_FRENCH:     return L_FR;
        case LANG_SPANISH:    return L_ES;
        case LANG_ITALIAN:    return L_IT;
        case LANG_PORTUGUESE: return L_PT;
        case LANG_RUSSIAN:    return L_RU;
        case LANG_JAPANESE:   return L_JA;
        case LANG_CHINESE:    return L_ZH;
        case LANG_KOREAN:     return L_KO;
        case LANG_DUTCH:      return L_NL;
        case LANG_POLISH:     return L_PL;
        case LANG_UKRAINIAN:  return L_UK;
        case LANG_ENGLISH:
        default:              return L_EN;   // default: English
    }
}
inline Lang lang() { static Lang g = detectLang(); return g; }

// Multilingual string type
// Order: { TR, EN, DE, FR, ES, IT, PT, RU, JA, ZH, KO, AR, NL, PL }
struct Txt {
    const wchar_t* v[L_COUNT];
    operator const wchar_t*() const { return v[lang()]; }
    operator std::wstring()   const { return v[lang()]; }
};

// ================================================================================
//  GENEL
// ================================================================================
inline const Txt APP_TITLE = {{
    L"Linux Mint Yükseltme Yardımcısı",
    L"Linux Mint Upgrade Assistant",
    L"Linux Mint Upgrade-Assistent",
    L"Assistant de mise à niveau Linux Mint",
    L"Asistente de actualización de Linux Mint",
    L"Assistente di aggiornamento Linux Mint",
    L"Assistente de atualização do Linux Mint",
    L"Помощник обновления Linux Mint",
     L"Linux Mint アップグレードアシスタント",
     L"Linux Mint 升级助手",
     L"Linux Mint 업그레이드 도우미",
     L"Linux Mint Upgrade-assistent",
     L"Asystent aktualizacji Linux Mint",
    L"Помічник оновлення Linux Mint" }};

inline const wchar_t* LOGO = L"Linux Mint";   // universal — not translated

inline const Txt EXIT_CONFIRM = {{
    L"Çıkmak istediğinizden emin misiniz?",
    L"Are you sure you want to exit?",
    L"Möchten Sie das Programm wirklich beenden?",
    L"Voulez-vous vraiment quitter ?",
    L"¿Está seguro de que desea salir?",
    L"Sei sicuro di voler uscire?",
    L"Tem certeza de que deseja sair?",
    L"Вы действительно хотите выйти?",
     L"終了してもよろしいですか？",
     L"确定要退出吗？",
     L"종료하시겠습니까?",
     L"Weet u zeker dat u wilt afsluiten?",
     L"Czy na pewno chcesz zakończyć?",
    L"Ви впевнені, що хочете вийти?" }};

// Confirmation asked when Cancel is pressed during installation
inline const Txt CANCEL_CONFIRM = {{
    L"Kurulumu iptal etmek istediğinizden emin misiniz?",
    L"Are you sure you want to cancel the installation?",
    L"Möchten Sie die Installation wirklich abbrechen?",
    L"Voulez-vous vraiment annuler l'installation ?",
    L"¿Está seguro de que desea cancelar la instalación?",
    L"Sei sicuro di voler annullare l'installazione?",
    L"Tem certeza de que deseja cancelar a instalação?",
    L"Вы действительно хотите отменить установку?",
    L"インストールをキャンセルしてもよろしいですか？",
    L"确定要取消安装吗？",
    L"설치를 취소하시겠습니까?",
    L"Weet u zeker dat u de installatie wilt annuleren?",
    L"Czy na pewno chcesz anulować instalację?",
    L"Ви впевнені, що хочете скасувати встановлення?" }};

// Top navigation
inline const Txt NAV[4] = {
  {{ L"Hazırlık",     L"Preparation",   L"Vorbereitung",  L"Préparation",   L"Preparación",   L"Preparazione",  L"Preparação",    L"Подготовка",
     L"準備",
     L"准备",
     L"준비",
     L"Voorbereiding",
     L"Przygotowanie",
    L"Підготовка" }},
  {{ L"Uyumluluk",    L"Compatibility", L"Kompatibilität",L"Compatibilité", L"Compatibilidad",L"Compatibilità", L"Compatibilidade",L"Совместимость",
     L"互換性",
     L"兼容性",
     L"호환성",
     L"Compatibiliteit",
     L"Zgodność",
    L"Сумісність" }},
  {{ L"Yükleme",      L"Install",       L"Installation",  L"Installation",  L"Instalación",   L"Installazione", L"Instalação",    L"Установка",
     L"インストール",
     L"安装",
     L"설치",
     L"Installatie",
     L"Instalacja",
    L"Встановлення" }},
  {{ L"Yapılandırma", L"Configuration", L"Konfiguration", L"Configuration", L"Configuración", L"Configurazione",L"Configuração",  L"Настройка",
     L"構成",
     L"配置",
     L"구성",
     L"Configuratie",
     L"Konfiguracja",
    L"Налаштування" }},
};

// ================================================================================
//  SAYFA: Sistem denetimi
// ================================================================================
inline const Txt CHK_TITLE_BUSY = {{
    L"Bilgisayarınız denetleniyor",
    L"Checking your PC",
    L"Ihr PC wird überprüft",
    L"Vérification de votre PC",
    L"Comprobando su PC",
    L"Controllo del PC in corso",
    L"Verificando seu PC",
    L"Проверка вашего компьютера",
     L"PC を確認しています",
     L"正在检查您的电脑",
     L"PC를 확인하는 중",
     L"Uw pc wordt gecontroleerd",
     L"Sprawdzanie komputera",
    L"Перевірка комп'ютера" }};

inline const Txt CHK_TITLE_DONE = {{
    L"Bilgisayarınıza göz attık",
    L"Here's what we found on your PC",
    L"Wir haben Ihren PC überprüft",
    L"Voici ce que nous avons trouvé",
    L"Esto es lo que encontramos",
    L"Ecco cosa abbiamo trovato",
    L"Aqui está o que encontramos",
    L"Вот что мы обнаружили",
     L"PC の確認結果",
     L"这是我们的发现",
     L"PC에서 찾은 내용",
     L"Dit hebben we gevonden",
     L"Oto co znaleźliśmy",
    L"Ось що ми знайшли на вашому ПК" }};

inline const Txt CHK_SUB_BUSY = {{
    L"Bu işlem yalnızca birkaç saniye sürecek.",
    L"This will only take a few seconds.",
    L"Dies dauert nur wenige Sekunden.",
    L"Cela ne prendra que quelques secondes.",
    L"Esto solo tomará unos segundos.",
    L"Ci vorranno solo pochi secondi.",
    L"Isso levará apenas alguns segundos.",
    L"Это займёт всего несколько секунд.",
     L"数秒で完了します。",
     L"这只需几秒钟。",
     L"몇 초밖에 걸리지 않습니다.",
     L"Dit duurt maar enkele seconden.",
     L"Zajmie to tylko kilka sekund.",
    L"Це займе лише кілька секунд." }};

inline const Txt CHK_BUSY_NET = {{
    L"En son Linux Mint sürümü sorgulanıyor…",
    L"Checking for the latest Linux Mint release…",
    L"Neueste Linux-Mint-Version wird ermittelt…",
    L"Recherche de la dernière version de Linux Mint…",
    L"Buscando la última versión de Linux Mint…",
    L"Ricerca dell'ultima versione di Linux Mint…",
    L"Verificando a versão mais recente do Linux Mint…",
    L"Проверка последней версии Linux Mint…",
     L"最新の Linux Mint を確認しています…",
     L"正在检查最新的 Linux Mint 版本…",
     L"최신 Linux Mint 릴리스를 확인하는 중…",
     L"Nieuwste Linux Mint-versie zoeken…",
     L"Sprawdzanie najnowszej wersji…",
    L"Пошук найновішої версії Linux Mint…" }};

inline const Txt CHK_ITEM_RAM = {{
    L"Bellek (RAM)", L"Memory (RAM)", L"Arbeitsspeicher (RAM)", L"Mémoire (RAM)",
    L"Memoria (RAM)", L"Memoria (RAM)", L"Memória (RAM)", L"Память (ОЗУ)",
     L"メモリ (RAM)",
     L"内存 (RAM)",
     L"메모리 (RAM)",
     L"Geheugen (RAM)",
     L"Pamięć (RAM)",
    L"Пам'ять (ОЗП)" }};

inline const Txt CHK_ITEM_CPU = {{
    L"İşlemci", L"Processor", L"Prozessor", L"Processeur",
    L"Procesador", L"Processore", L"Processador", L"Процессор",
     L"プロセッサ",
     L"处理器",
     L"프로세서",
     L"Processor",
     L"Procesor",
    L"Процесор" }};

inline const Txt CHK_ITEM_DISK = {{
    L"Boş depolama alanı (C:)", L"Free storage space (C:)", L"Freier Speicherplatz (C:)",
    L"Espace libre (C:)", L"Espacio libre (C:)", L"Spazio libero (C:)",
    L"Espaço livre (C:)", L"Свободное место (C:)",
     L"空き容量 (C:)",
     L"可用空间 (C:)",
     L"여유 공간 (C:)",
     L"Vrije ruimte (C:)",
     L"Wolne miejsce (C:)",
    L"Вільне місце (C:)" }};

inline const Txt CHK_ITEM_FW = {{
    L"Önyükleme türü", L"Boot mode", L"Startmodus", L"Mode de démarrage",
    L"Modo de arranque", L"Modalità di avvio", L"Modo de inicialização", L"Режим загрузки",
     L"起動モード",
     L"启动模式",
     L"부팅 모드",
     L"Opstartmodus",
     L"Tryb rozruchu",
    L"Режим завантаження" }};

inline const Txt CHK_ITEM_SB = {{
    L"Secure Boot", L"Secure Boot", L"Secure Boot", L"Secure Boot",
    L"Secure Boot", L"Secure Boot", L"Secure Boot", L"Secure Boot",
     L"Secure Boot",
     L"Secure Boot",
     L"Secure Boot",
     L"Secure Boot",
     L"Secure Boot",
    L"Secure Boot" }};

inline const Txt VAL_ON = {{
    L"Açık", L"On", L"Ein", L"Activé", L"Activado", L"Attivo", L"Ativado", L"Включено",
     L"オン",
     L"开",
     L"켜짐",
     L"Aan",
     L"Wł.",
    L"Увімкнено" }};

inline const Txt VAL_OFF = {{
    L"Kapalı", L"Off", L"Aus", L"Désactivé", L"Desactivado", L"Disattivo", L"Desativado", L"Выключено",
     L"オフ",
     L"关",
     L"꺼짐",
     L"Uit",
     L"Wył.",
    L"Вимкнено" }};

inline const Txt WORD_CORES = {{
    L"çekirdek", L"cores", L"Kerne", L"cœurs", L"núcleos", L"core", L"núcleos", L"ядер",
     L"コア",
     L"核心",
     L"코어",
     L"kernen",
     L"rdzeni",
    L"ядер" }};

inline const Txt WORD_APPROX = {{
    L"yaklaşık", L"approx.", L"ca.", L"env.", L"aprox.", L"circa", L"aprox.", L"примерно",
     L"約",
     L"约",
     L"약",
     L"ca.",
     L"ok.",
    L"прибл." }};

inline const Txt CHK_RECOMMEND = {{
    L"Önerilen sürüm",
    L"Recommended edition",
    L"Empfohlene Edition",
    L"Édition recommandée",
    L"Edición recomendada",
    L"Edizione consigliata",
    L"Edição recomendada",
    L"Рекомендуемая редакция",
     L"推奨エディション",
     L"推荐版本",
     L"권장 에디션",
     L"Aanbevolen editie",
     L"Zalecana edycja",
    L"Рекомендована редакція" }};

inline const Txt CHK_DL_NOTE = {{
    L"İleri'ye bastığınızda kurulum dosyaları (ISO) arka planda indirilmeye başlar.",
    L"When you press Next, the installation files (ISO) will start downloading in the background.",
    L"Wenn Sie auf Weiter klicken, werden die Installationsdateien (ISO) im Hintergrund heruntergeladen.",
    L"En cliquant sur Suivant, les fichiers d'installation (ISO) seront téléchargés en arrière-plan.",
    L"Al pulsar Siguiente, los archivos de instalación (ISO) comenzarán a descargarse en segundo plano.",
    L"Premendo Avanti, i file di installazione (ISO) inizieranno a scaricarsi in background.",
    L"Ao clicar em Avançar, os arquivos de instalação (ISO) começarão a ser baixados em segundo plano.",
    L"После нажатия «Далее» файлы установки (ISO) начнут загружаться в фоновом режиме.",
     L"「次へ」を押すと、インストールファイル (ISO) がバックグラウンドでダウンロードされます。",
     L"按“下一步”后，安装文件 (ISO) 将在后台开始下载。",
     L"다음을 누르면 설치 파일(ISO)이 백그라운드에서 다운로드됩니다.",
     L"Als u op Volgende klikt, worden de installatiebestanden (ISO) op de achtergrond gedownload.",
     L"Po naciśnięciu „Dalej” pliki instalacyjne (ISO) zaczną pobierać się w tle.",
    L"Після натискання «Далі» файли встановлення (ISO) почнуть завантажуватися у фоновому режимі." }};

inline const Txt CHK_ERR_UEFI = {{
    L"Bu bilgisayar Legacy/BIOS modunda önyükleniyor. Yardımcı yalnızca UEFI sistemlerde çalışır.",
    L"This PC boots in Legacy/BIOS mode. The assistant only works on UEFI systems.",
    L"Dieser PC startet im Legacy/BIOS-Modus. Der Assistent funktioniert nur auf UEFI-Systemen.",
    L"Ce PC démarre en mode Legacy/BIOS. L'assistant ne fonctionne que sur les systèmes UEFI.",
    L"Este PC arranca en modo Legacy/BIOS. El asistente solo funciona en sistemas UEFI.",
    L"Questo PC si avvia in modalità Legacy/BIOS. L'assistente funziona solo su sistemi UEFI.",
    L"Este PC inicializa em modo Legacy/BIOS. O assistente funciona apenas em sistemas UEFI.",
    L"Этот ПК загружается в режиме Legacy/BIOS. Помощник работает только на системах UEFI.",
     L"この PC は Legacy/BIOS モードで起動しています。このアシスタントは UEFI システムでのみ動作します。",
     L"此电脑以 Legacy/BIOS 模式启动。此助手仅在 UEFI 系统上运行。",
     L"이 PC는 Legacy/BIOS 모드로 부팅됩니다. 이 도우미는 UEFI 시스템에서만 작동합니다.",
     L"Deze pc start op in Legacy/BIOS-modus. De assistent werkt alleen op UEFI-systemen.",
     L"Ten komputer uruchamia się w trybie Legacy/BIOS. Asystent działa tylko w systemach UEFI.",
    L"Цей ПК завантажується в режимі Legacy/BIOS. Помічник працює лише на системах UEFI." }};

inline const Txt CHK_ERR_SB = {{
    L"Secure Boot açık. Devam etmek için BIOS/UEFI ayarlarından Secure Boot'u kapatıp uygulamayı yeniden çalıştırın.",
    L"Secure Boot is enabled. To continue, disable Secure Boot in your BIOS/UEFI settings and restart the app.",
    L"Secure Boot ist aktiviert. Deaktivieren Sie Secure Boot in den BIOS/UEFI-Einstellungen und starten Sie die App neu.",
    L"Secure Boot est activé. Désactivez-le dans les paramètres BIOS/UEFI puis relancez l'application.",
    L"Secure Boot está activado. Desactívelo en la configuración BIOS/UEFI y reinicie la aplicación.",
    L"Secure Boot è attivo. Disattivalo nelle impostazioni BIOS/UEFI e riavvia l'applicazione.",
    L"O Secure Boot está ativado. Desative-o nas configurações do BIOS/UEFI e reinicie o aplicativo.",
    L"Включён Secure Boot. Отключите его в настройках BIOS/UEFI и перезапустите приложение.",
     L"Secure Boot が有効です。続行するには BIOS/UEFI 設定で Secure Boot を無効にしてアプリを再起動してください。",
     L"已启用 Secure Boot。请在 BIOS/UEFI 设置中禁用它，然后重新启动应用。",
     L"Secure Boot가 켜져 있습니다. 계속하려면 BIOS/UEFI 설정에서 끄고 앱을 다시 시작하세요.",
     L"Secure Boot is ingeschakeld. Schakel het uit in de BIOS/UEFI-instellingen en herstart de app.",
     L"Secure Boot jest włączony. Wyłącz go w ustawieniach BIOS/UEFI i uruchom aplikację ponownie.",
    L"Secure Boot увімкнено. Щоб продовжити, вимкніть Secure Boot у налаштуваннях BIOS/UEFI та перезапустіть застосунок." }};

inline const Txt CHK_ERR_DISK = {{
    L"C: sürücüsünde en az 15 GB boş alan gerekiyor.",
    L"At least 15 GB of free space is required on drive C:.",
    L"Auf Laufwerk C: sind mindestens 15 GB freier Speicherplatz erforderlich.",
    L"Au moins 15 Go d'espace libre sont requis sur le lecteur C:.",
    L"Se requieren al menos 15 GB de espacio libre en la unidad C:.",
    L"Sono necessari almeno 15 GB di spazio libero sull'unità C:.",
    L"São necessários pelo menos 15 GB de espaço livre na unidade C:.",
    L"На диске C: требуется не менее 15 ГБ свободного места.",
     L"C: ドライブに少なくとも 15 GB の空き容量が必要です。",
     L"C: 盘至少需要 15 GB 可用空间。",
     L"C: 드라이브에 최소 15GB의 여유 공간이 필요합니다.",
     L"Op station C: is minstens 15 GB vrije ruimte vereist.",
     L"Na dysku C: wymagane jest co najmniej 15 GB wolnego miejsca.",
    L"На диску C: потрібно щонайменше 15 ГБ вільного місця." }};

inline const Txt CHK_ERR_64 = {{
    L"Bu uygulama yalnızca 64-bit (x64) sistemlerde çalışır.",
    L"This application only runs on 64-bit (x64) systems.",
    L"Diese Anwendung läuft nur auf 64-Bit-Systemen (x64).",
    L"Cette application ne fonctionne que sur les systèmes 64 bits (x64).",
    L"Esta aplicación solo funciona en sistemas de 64 bits (x64).",
    L"Questa applicazione funziona solo su sistemi a 64 bit (x64).",
    L"Este aplicativo funciona apenas em sistemas de 64 bits (x64).",
    L"Это приложение работает только на 64-битных системах (x64).",
     L"このアプリは 64 ビット (x64) システムでのみ動作します。",
     L"此应用仅在 64 位 (x64) 系统上运行。",
     L"이 앱은 64비트(x64) 시스템에서만 실행됩니다.",
     L"Deze app werkt alleen op 64-bits (x64) systemen.",
     L"Ta aplikacja działa tylko w systemach 64-bitowych (x64).",
    L"Цей застосунок працює лише на 64-розрядних системах (x64)." }};

inline const Txt CHK_WARN_RAM = {{
    L"Not: RAM önerilen 2 GB'ın altında. Kurulum yine de yapılabilir; en hafif sürüm (Xfce) seçildi ancak yavaşlık yaşanabilir.",
    L"Note: RAM is below the recommended 2 GB. You can still install; the lightest edition (Xfce) is selected, but it may run slowly.",
    L"Hinweis: RAM liegt unter den empfohlenen 2 GB. Die Installation ist möglich; die leichteste Edition (Xfce) ist gewählt, kann aber langsam laufen.",
    L"Remarque : la RAM est inférieure aux 2 Go recommandés. L'installation reste possible ; l'édition la plus légère (Xfce) est sélectionnée, mais des lenteurs sont possibles.",
    L"Nota: la RAM está por debajo de los 2 GB recomendados. Aún puede instalar; se seleccionó la edición más ligera (Xfce), pero puede ir lento.",
    L"Nota: la RAM è inferiore ai 2 GB consigliati. Puoi comunque installare; è selezionata l'edizione più leggera (Xfce), ma potrebbe essere lenta.",
    L"Nota: a RAM está abaixo dos 2 GB recomendados. Ainda é possível instalar; a edição mais leve (Xfce) foi selecionada, mas pode ficar lenta.",
    L"Примечание: ОЗУ меньше рекомендуемых 2 ГБ. Установка возможна; выбрана самая лёгкая редакция (Xfce), но система может работать медленно.",
     L"注意: RAM が推奨の 2 GB を下回っています。インストールは可能ですが、最軽量エディション (Xfce) が選択され、動作が遅くなる場合があります。",
     L"注意：内存低于建议的 2 GB。仍可安装；已选择最轻量版本 (Xfce)，但可能运行缓慢。",
     L"참고: RAM이 권장 2GB 미만입니다. 설치는 가능하며 가장 가벼운 에디션(Xfce)이 선택되었지만 느릴 수 있습니다.",
     L"Let op: RAM ligt onder de aanbevolen 2 GB. Installeren kan; de lichtste editie (Xfce) is gekozen, maar kan traag zijn.",
     L"Uwaga: RAM poniżej zalecanych 2 GB. Instalacja możliwa; wybrano najlżejszą edycję (Xfce), ale może działać wolno.",
    L"Примітка: ОЗП менше рекомендованих 2 ГБ. Встановлення можливе; вибрано найлегшу редакцію (Xfce), але робота може бути повільною." }};

inline const Txt CHK_ED_RECOMMEND = {{
    L"önerilen", L"recommended", L"empfohlen", L"recommandé",
    L"recomendada", L"consigliata", L"recomendada", L"рекомендуется",
     L"推奨",
     L"推荐",
     L"권장",
     L"aanbevolen",
     L"zalecane",
    L"рекомендовано" }};

inline const Txt CHK_ED_WARN_RAM = {{
    L"donanımınız için ağır olabilir",
    L"may be heavy for your hardware",
    L"könnte für Ihre Hardware zu schwer sein",
    L"peut être lourd pour votre matériel",
    L"puede ser pesada para su hardware",
    L"potrebbe essere pesante per il tuo hardware",
    L"pode ser pesada para seu hardware",
    L"может быть тяжёлой для вашего оборудования",
     L"お使いのハードウェアには重い可能性があります",
     L"可能对您的硬件来说较重",
     L"하드웨어에 부담이 될 수 있음",
     L"kan zwaar zijn voor uw hardware",
     L"może być zbyt wymagająca",
    L"може бути важкою для вашого обладнання" }};

inline const Txt CHK_LINK_OTHER = {{
    L"Başka bir sürüm seç",
    L"Choose a different edition",
    L"Andere Edition wählen",
    L"Choisir une autre édition",
    L"Elegir otra edición",
    L"Scegli un'altra edizione",
    L"Escolher outra edição",
    L"Выбрать другую редакцию",
     L"別のエディションを選択",
     L"选择其他版本",
     L"다른 에디션 선택",
     L"Andere editie kiezen",
     L"Wybierz inną edycję",
    L"Вибрати іншу редакцію" }};

// Edition blurbs (in the EDITIONS order from Config: Cinnamon, MATE, Xfce)
inline const Txt ED_BLURB[3] = {
  {{ L"Modern ve tam donanımlı masaüstü. Donanımınız bunun için fazlasıyla yeterli.",
     L"Modern, full-featured desktop. Your hardware is more than enough for it.",
     L"Moderner, voll ausgestatteter Desktop. Ihre Hardware ist dafür mehr als ausreichend.",
     L"Bureau moderne et complet. Votre matériel est largement suffisant.",
     L"Escritorio moderno y completo. Su hardware es más que suficiente.",
     L"Desktop moderno e completo. Il tuo hardware è più che sufficiente.",
     L"Área de trabalho moderna e completa. Seu hardware é mais que suficiente.",
     L"Современный полнофункциональный рабочий стол. Вашего оборудования более чем достаточно.",
     L"モダンでフル機能のデスクトップ。お使いのハードウェアには十分すぎます。",
     L"现代化、功能齐全的桌面。您的硬件绰绰有余。",
     L"현대적이고 완전한 기능의 데스크톱. 하드웨어가 충분합니다.",
     L"Moderne, volledige desktop. Uw hardware is ruim voldoende.",
     L"Nowoczesny, w pełni funkcjonalny pulpit. Twój sprzęt w zupełności wystarcza.",
    L"Сучасне повнофункціональне середовище. Вашого обладнання більш ніж достатньо." }},
  {{ L"Klasik ve dengeli masaüstü. Orta seviye donanımlar için ideal.",
     L"Classic, balanced desktop. Ideal for mid-range hardware.",
     L"Klassischer, ausgewogener Desktop. Ideal für Mittelklasse-Hardware.",
     L"Bureau classique et équilibré. Idéal pour le matériel de milieu de gamme.",
     L"Escritorio clásico y equilibrado. Ideal para hardware de gama media.",
     L"Desktop classico ed equilibrato. Ideale per hardware di fascia media.",
     L"Área de trabalho clássica e equilibrada. Ideal para hardware intermediário.",
     L"Классический сбалансированный рабочий стол. Идеален для среднего оборудования.",
     L"クラシックでバランスの取れたデスクトップ。中程度のハードウェアに最適。",
     L"经典、均衡的桌面。适合中端硬件。",
     L"클래식하고 균형 잡힌 데스크톱. 중급 하드웨어에 이상적.",
     L"Klassiek, gebalanceerd bureaublad. Ideaal voor middenklasse hardware.",
     L"Klasyczny, zrównoważony pulpit. Idealny dla sprzętu średniej klasy.",
    L"Класичне збалансоване середовище. Ідеально для обладнання середнього класу." }},
  {{ L"En hafif sürüm. Düşük RAM'li sistemlerde bile akıcı çalışır.",
     L"The lightest edition. Runs smoothly even on low-RAM systems.",
     L"Die leichteste Edition. Läuft selbst auf Systemen mit wenig RAM flüssig.",
     L"L'édition la plus légère. Fluide même avec peu de RAM.",
     L"La edición más ligera. Fluida incluso con poca RAM.",
     L"L'edizione più leggera. Fluida anche con poca RAM.",
     L"A edição mais leve. Fluida mesmo com pouca RAM.",
     L"Самая лёгкая редакция. Работает плавно даже при малом объёме ОЗУ.",
     L"最軽量エディション。RAM が少ないシステムでもスムーズに動作します。",
     L"最轻量版本。即使内存较低也能流畅运行。",
     L"가장 가벼운 에디션. RAM이 적은 시스템에서도 부드럽게 실행됩니다.",
     L"De lichtste editie. Werkt vloeiend, zelfs met weinig RAM.",
     L"Najlżejsza edycja. Działa płynnie nawet przy małej ilości RAM.",
    L"Найлегша редакція. Працює плавно навіть на системах з малим об'ємом ОЗП." }},
};

// ================================================================================
//  PAGE: Compatibility scan + result
// ================================================================================
inline const Txt CMP_TITLE = {{
    L"Uyumlu olanlara bakalım",
    L"Let's see what's compatible",
    L"Sehen wir, was kompatibel ist",
    L"Voyons ce qui est compatible",
    L"Veamos qué es compatible",
    L"Vediamo cosa è compatibile",
    L"Vamos ver o que é compatível",
    L"Посмотрим, что совместимо",
     L"互換性を確認しましょう",
     L"看看有哪些兼容",
     L"호환되는 항목 보기",
     L"Laten we kijken wat compatibel is",
     L"Zobaczmy, co jest zgodne",
    L"Подивимося, що сумісне" }};

inline const Txt CMP_SUB = {{
    L"Bu işlem birkaç dakika sürecek.",
    L"This may take a few minutes.",
    L"Dies kann einige Minuten dauern.",
    L"Cela peut prendre quelques minutes.",
    L"Esto puede tardar unos minutos.",
    L"Potrebbe richiedere alcuni minuti.",
    L"Isso pode levar alguns minutos.",
    L"Это может занять несколько минут.",
     L"数分かかる場合があります。",
     L"这可能需要几分钟。",
     L"몇 분 정도 걸릴 수 있습니다.",
     L"Dit kan enkele minuten duren.",
     L"Może to potrwać kilka minut.",
    L"Це може зайняти кілька хвилин." }};

inline const Txt CMP_SCANNING = {{
    L"Uygulamalar ve aygıtlar denetleniyor",
    L"Checking your apps and devices",
    L"Apps und Geräte werden überprüft",
    L"Vérification de vos applications et appareils",
    L"Comprobando sus aplicaciones y dispositivos",
    L"Controllo di app e dispositivi",
    L"Verificando seus aplicativos e dispositivos",
    L"Проверка приложений и устройств",
     L"アプリとデバイスを確認しています",
     L"正在检查应用和设备",
     L"앱 및 장치 확인 중",
     L"Apps en apparaten controleren",
     L"Sprawdzanie aplikacji i urządzeń",
    L"Перевірка застосунків і пристроїв" }};

inline const Txt RES_TITLE = {{
    L"İşte bulduklarımız",
    L"Here's what we found",
    L"Das haben wir gefunden",
    L"Voici ce que nous avons trouvé",
    L"Esto es lo que encontramos",
    L"Ecco cosa abbiamo trovato",
    L"Aqui está o que encontramos",
    L"Вот что мы нашли",
     L"確認結果",
     L"这是我们的发现",
     L"찾은 내용",
     L"Dit hebben we gevonden",
     L"Oto co znaleźliśmy",
    L"Ось що ми знайшли" }};

inline const Txt RES_SUB = {{
    L"Uyumlulukla ilgili ayrıntılarda her uygulama hakkında daha fazla bilgi edinebilirsiniz.",
    L"You can learn more about each app in the compatibility details.",
    L"In den Kompatibilitätsdetails erfahren Sie mehr über jede App.",
    L"Consultez les détails de compatibilité pour en savoir plus sur chaque application.",
    L"Puede obtener más información sobre cada aplicación en los detalles de compatibilidad.",
    L"Puoi saperne di più su ogni app nei dettagli di compatibilità.",
    L"Você pode saber mais sobre cada aplicativo nos detalhes de compatibilidade.",
    L"Подробнее о каждом приложении — в сведениях о совместимости.",
     L"互換性の詳細で各アプリについてさらに確認できます。",
     L"您可以在兼容性详情中了解每个应用的更多信息。",
     L"호환성 세부 정보에서 각 앱에 대해 자세히 알아볼 수 있습니다.",
     L"U kunt meer lezen over elke app in de compatibiliteitsdetails.",
     L"Więcej o każdej aplikacji znajdziesz w szczegółach zgodności.",
    L"Докладніше про кожен застосунок — у відомостях про сумісність." }};

inline const Txt RES_OK_FMT = {{
    L"%d uygulamanız ve aygıtınız uyumlu",
    L"%d of your apps and devices are compatible",
    L"%d Ihrer Apps und Geräte sind kompatibel",
    L"%d de vos applications et appareils sont compatibles",
    L"%d de sus aplicaciones y dispositivos son compatibles",
    L"%d delle tue app e dei tuoi dispositivi sono compatibili",
    L"%d dos seus aplicativos e dispositivos são compatíveis",
    L"Совместимо приложений и устройств: %d",
     L"%d 個のアプリとデバイスが互換",
     L"%d 个应用和设备兼容",
     L"%d개의 앱 및 장치가 호환됨",
     L"%d van uw apps en apparaten zijn compatibel",
     L"%d aplikacji i urządzeń jest zgodnych",
    L"Сумісних застосунків і пристроїв: %d" }};

inline const Txt RES_BAD_FMT = {{
    L"%d öğeyi gözden geçirmeniz gerekiyor",
    L"%d item(s) need your review",
    L"%d Element(e) sollten Sie prüfen",
    L"%d élément(s) à examiner",
    L"%d elemento(s) requieren revisión",
    L"%d elemento/i da esaminare",
    L"%d item(ns) precisam de revisão",
    L"Требуют внимания: %d",
     L"%d 個の項目を確認する必要があります",
     L"%d 个项目需要您检查",
     L"%d개 항목을 검토해야 합니다",
     L"%d item(s) vereisen uw aandacht",
     L"%d element(ów) wymaga sprawdzenia",
    L"Потребують уваги: %d" }};

inline const Txt RES_LINK = {{
    L"Uyumluluk ayrıntılarına bakın",
    L"See compatibility details",
    L"Kompatibilitätsdetails anzeigen",
    L"Voir les détails de compatibilité",
    L"Ver detalles de compatibilidad",
    L"Vedi i dettagli di compatibilità",
    L"Ver detalhes de compatibilidade",
    L"Посмотреть сведения о совместимости",
     L"互換性の詳細を見る",
     L"查看兼容性详情",
     L"호환성 세부 정보 보기",
     L"Compatibiliteitsdetails bekijken",
     L"Zobacz szczegóły zgodności",
    L"Переглянути відомості про сумісність" }};

inline const Txt RES_ALT_LABEL = {{
    L"önerilen alternatif",
    L"suggested alternative",
    L"empfohlene Alternative",
    L"alternative suggérée",
    L"alternativa sugerida",
    L"alternativa suggerita",
    L"alternativa sugerida",
    L"рекомендуемая альтернатива",
     L"推奨される代替",
     L"建议的替代",
     L"권장 대체",
     L"voorgesteld alternatief",
     L"sugerowana alternatywa",
    L"рекомендована альтернатива" }};

// ================================================================================
//  PAGE: What to keep
// ================================================================================
inline const Txt KEEP_TITLE = {{
    L"Neleri saklamak istersiniz?",
    L"What would you like to keep?",
    L"Was möchten Sie behalten?",
    L"Que souhaitez-vous conserver ?",
    L"¿Qué desea conservar?",
    L"Cosa vuoi conservare?",
    L"O que você deseja manter?",
    L"Что вы хотите сохранить?",
     L"何を残しますか？",
     L"您想保留什么？",
     L"무엇을 유지하시겠습니까?",
     L"Wat wilt u behouden?",
     L"Co chcesz zachować?",
    L"Що ви хочете зберегти?" }};

inline const Txt KEEP_FILES = {{
    L"Kullanıcı hesapları ve kişisel dosyalar",
    L"User accounts and personal files",
    L"Benutzerkonten und persönliche Dateien",
    L"Comptes utilisateur et fichiers personnels",
    L"Cuentas de usuario y archivos personales",
    L"Account utente e file personali",
    L"Contas de usuário e arquivos pessoais",
    L"Учётные записи и личные файлы",
     L"ユーザーアカウントと個人ファイル",
     L"用户账户和个人文件",
     L"사용자 계정 및 개인 파일",
     L"Gebruikersaccounts en persoonlijke bestanden",
     L"Konta użytkowników i pliki osobiste",
    L"Облікові записи та особисті файли" }};

inline const Txt KEEP_FILES_SOON = {{
    L"(yakında)", L"(coming soon)", L"(demnächst)", L"(bientôt)",
    L"(próximamente)", L"(in arrivo)", L"(em breve)", L"(скоро)",
     L"（近日対応）",
     L"（即将推出）",
     L"(곧 제공)",
     L"(binnenkort)",
     L"(wkrótce)",
    L"(незабаром)" }};

inline const Txt KEEP_NOTHING = {{
    L"Hiçbir şey", L"Nothing", L"Nichts", L"Rien",
    L"Nada", L"Niente", L"Nada", L"Ничего",
     L"何も残さない",
     L"不保留",
     L"아무것도 안 함",
     L"Niets",
     L"Nic",
    L"Нічого" }};

// ================================================================================
//  PAGE: Ready to install
// ================================================================================
inline const Txt RDY_TITLE = {{
    L"Yüklemeye hazır",
    L"Ready to install",
    L"Bereit zur Installation",
    L"Prêt à installer",
    L"Listo para instalar",
    L"Pronto per l'installazione",
    L"Pronto para instalar",
    L"Готово к установке",
     L"インストールの準備完了",
     L"准备安装",
     L"설치 준비 완료",
     L"Klaar om te installeren",
     L"Gotowe do instalacji",
    L"Готово до встановлення" }};

inline const Txt RDY_SUB = {{
    L"Kurulum ortamı hazırlanırken bilgisayarınızı kullanmaya devam edebilirsiniz. Özetlemek gerekirse, seçtikleriniz:",
    L"You can keep using your PC while the installation media is prepared. To recap, here's what you've chosen:",
    L"Sie können Ihren PC weiter nutzen, während das Installationsmedium vorbereitet wird. Zusammengefasst haben Sie gewählt:",
    L"Vous pouvez continuer à utiliser votre PC pendant la préparation. En résumé, voici vos choix :",
    L"Puede seguir usando su PC mientras se prepara el medio de instalación. En resumen, ha elegido:",
    L"Puoi continuare a usare il PC durante la preparazione. In sintesi, hai scelto:",
    L"Você pode continuar usando o PC enquanto a mídia é preparada. Em resumo, você escolheu:",
    L"Пока идёт подготовка, вы можете пользоваться компьютером. Итак, вы выбрали:",
     L"インストールメディアの準備中も PC を使い続けられます。選択内容は次のとおりです:",
     L"准备安装介质时您可以继续使用电脑。回顾一下您的选择：",
     L"설치 미디어가 준비되는 동안 PC를 계속 사용할 수 있습니다. 선택 사항은 다음과 같습니다:",
     L"U kunt uw pc blijven gebruiken terwijl de installatiemedia worden voorbereid. Uw keuzes:",
     L"Możesz nadal używać komputera podczas przygotowywania nośnika. Twoje wybory:",
    L"Ви можете користуватися ПК, поки готується носій встановлення. Отже, ви обрали:" }};

inline const Txt RDY_KEEP_NOTH = {{
    L"Hiçbir şeyi saklama", L"Keep nothing", L"Nichts behalten", L"Ne rien conserver",
    L"No conservar nada", L"Non conservare nulla", L"Não manter nada", L"Ничего не сохранять",
     L"何も残さない",
     L"不保留任何内容",
     L"아무것도 유지 안 함",
     L"Niets behouden",
     L"Nie zachowuj niczego",
    L"Нічого не зберігати" }};

inline const Txt RDY_NOUSB = {{
    L"USB bellek gerekmeden önyükleme (Windows Boot Manager + GRUB)",
    L"Boot without a USB drive (Windows Boot Manager + GRUB)",
    L"Start ohne USB-Stick (Windows Boot Manager + GRUB)",
    L"Démarrage sans clé USB (Windows Boot Manager + GRUB)",
    L"Arranque sin USB (Windows Boot Manager + GRUB)",
    L"Avvio senza chiavetta USB (Windows Boot Manager + GRUB)",
    L"Inicialização sem pen drive (Windows Boot Manager + GRUB)",
    L"Загрузка без USB-накопителя (Windows Boot Manager + GRUB)",
     L"USB メモリ不要で起動 (Windows Boot Manager + GRUB)",
     L"无需 U 盘即可启动 (Windows Boot Manager + GRUB)",
     L"USB 없이 부팅 (Windows Boot Manager + GRUB)",
     L"Opstarten zonder USB (Windows Boot Manager + GRUB)",
     L"Rozruch bez USB (Windows Boot Manager + GRUB)",
    L"Завантаження без USB-носія (Windows Boot Manager + GRUB)" }};

inline const Txt RDY_VER_ONLINE = {{
    L"Sürüm bilgisi çevrimiçi doğrulandı — en yeni kararlı sürüm.",
    L"Version verified online — the latest stable release.",
    L"Version online verifiziert — die neueste stabile Version.",
    L"Version vérifiée en ligne — la dernière version stable.",
    L"Versión verificada en línea — la última versión estable.",
    L"Versione verificata online — l'ultima versione stabile.",
    L"Versão verificada on-line — a versão estável mais recente.",
    L"Версия проверена онлайн — последняя стабильная.",
     L"バージョンをオンラインで確認 — 最新の安定版です。",
     L"已在线验证版本 — 最新稳定版。",
     L"온라인으로 확인됨 — 최신 안정 릴리스.",
     L"Versie online geverifieerd — de nieuwste stabiele release.",
     L"Wersja zweryfikowana online — najnowsze wydanie stabilne.",
    L"Версію перевірено онлайн — найновіший стабільний випуск." }};

inline const Txt RDY_VER_OFFLINE = {{
    L"Çevrimiçi sürüm sorgusu yapılamadı; bilinen son sürüm kullanılacak.",
    L"Online version check failed; the last known release will be used.",
    L"Online-Prüfung fehlgeschlagen; die letzte bekannte Version wird verwendet.",
    L"Vérification en ligne impossible ; la dernière version connue sera utilisée.",
    L"No se pudo verificar en línea; se usará la última versión conocida.",
    L"Verifica online non riuscita; verrà usata l'ultima versione nota.",
    L"Falha na verificação on-line; será usada a última versão conhecida.",
    L"Онлайн-проверка не удалась; будет использована последняя известная версия.",
     L"オンラインのバージョン確認に失敗しました。最後の既知のバージョンを使用します。",
     L"在线版本检查失败；将使用最后已知版本。",
     L"온라인 버전 확인 실패; 마지막으로 알려진 릴리스를 사용합니다.",
     L"Online versiecontrole mislukt; de laatst bekende release wordt gebruikt.",
     L"Sprawdzanie wersji online nie powiodło się; użyta zostanie ostatnia znana wersja.",
    L"Онлайн-перевірка не вдалася; буде використано останню відому версію." }};

inline const Txt RDY_DL_FMT = {{                        // printf: %d = percent
    L"ISO arka planda indiriliyor…  %%%d",
    L"Downloading ISO in the background…  %d%%",
    L"ISO wird im Hintergrund heruntergeladen…  %d %%",
    L"Téléchargement de l'ISO en arrière-plan…  %d %%",
    L"Descargando la ISO en segundo plano…  %d %%",
    L"Download dell'ISO in background…  %d%%",
    L"Baixando a ISO em segundo plano…  %d%%",
    L"Загрузка ISO в фоновом режиме…  %d %%",
     L"ISO をバックグラウンドでダウンロード中…  %d%%",
     L"正在后台下载 ISO…  %d%%",
     L"백그라운드에서 ISO 다운로드 중…  %d%%",
     L"ISO downloaden op achtergrond…  %d%%",
     L"Pobieranie ISO w tle…  %d%%",
    L"Завантаження ISO у фоновому режимі…  %d%%" }};

inline const Txt RDY_DL_DONE = {{
    L"ISO indirildi ve doğrulamaya hazır  ✓",
    L"ISO downloaded and ready for verification  ✓",
    L"ISO heruntergeladen und bereit zur Prüfung  ✓",
    L"ISO téléchargée, prête pour la vérification  ✓",
    L"ISO descargada y lista para verificar  ✓",
    L"ISO scaricata e pronta per la verifica  ✓",
    L"ISO baixada e pronta para verificação  ✓",
    L"ISO загружен и готов к проверке  ✓",
     L"ISO をダウンロードし、検証の準備が整いました  ✓",
     L"ISO 已下载，准备验证  ✓",
     L"ISO 다운로드 완료, 검증 준비됨  ✓",
     L"ISO gedownload en klaar voor verificatie  ✓",
     L"ISO pobrany i gotowy do weryfikacji  ✓",
    L"ISO завантажено та готово до перевірки  ✓" }};

inline const Txt RDY_DL_ERR = {{
    L"İndirme hatası oluştu — Yükle'ye bastığınızda yeniden denenecek.",
    L"A download error occurred — it will retry when you press Install.",
    L"Downloadfehler — beim Klick auf Installieren wird es erneut versucht.",
    L"Erreur de téléchargement — nouvelle tentative en cliquant sur Installer.",
    L"Error de descarga — se reintentará al pulsar Instalar.",
    L"Errore di download — verrà ritentato premendo Installa.",
    L"Erro de download — será tentado novamente ao clicar em Instalar.",
    L"Ошибка загрузки — повтор при нажатии «Установить».",
     L"ダウンロードエラーが発生しました — 「インストール」を押すと再試行します。",
     L"下载出错 — 按“安装”将重试。",
     L"다운로드 오류 — 설치를 누르면 다시 시도합니다.",
     L"Downloadfout — het wordt opnieuw geprobeerd bij Installeren.",
     L"Błąd pobierania — ponowna próba po naciśnięciu „Zainstaluj”.",
    L"Помилка завантаження — повтор при натисканні «Встановити»." }};

// ================================================================================
//  PAGE: Install ("Preparing %..") + splash
// ================================================================================
inline const Txt INS_BIG_FMT = {{                       // printf: %d = percent
    L"Hazırlanıyor  %%%d",
    L"Preparing  %d%%",
    L"Wird vorbereitet  %d %%",
    L"Préparation  %d %%",
    L"Preparando  %d %%",
    L"Preparazione  %d%%",
    L"Preparando  %d%%",
    L"Подготовка  %d %%",
     L"準備中  %d%%",
     L"正在准备  %d%%",
     L"준비 중  %d%%",
     L"Voorbereiden  %d%%",
     L"Przygotowywanie  %d%%",
    L"Підготовка  %d%%" }};

inline const Txt SPL_BIG_FMT = {{                       // splash screen, printf
    L"Hazırlanıyor  %%%d",
    L"Preparing  %d%%",
    L"Wird vorbereitet  %d %%",
    L"Préparation  %d %%",
    L"Preparando  %d %%",
    L"Preparazione  %d%%",
    L"Preparando  %d%%",
    L"Подготовка  %d %%",
     L"準備中  %d%%",
     L"正在准备  %d%%",
     L"준비 중  %d%%",
     L"Voorbereiden  %d%%",
     L"Przygotowywanie  %d%%",
    L"Підготовка  %d%%" }};

inline const Txt INS_PHASES[8] = {
  {{ L"Hazırlanıyor…", L"Preparing…", L"Wird vorbereitet…", L"Préparation…",
     L"Preparando…", L"Preparazione…", L"Preparando…", L"Подготовка…",
     L"準備中…",
     L"正在准备…",
     L"준비 중…",
     L"Voorbereiden…",
     L"Przygotowywanie…",
    L"Підготовка…" }},
  {{ L"ISO dosyası indiriliyor…", L"Downloading the ISO file…", L"ISO-Datei wird heruntergeladen…",
     L"Téléchargement du fichier ISO…", L"Descargando el archivo ISO…",
     L"Download del file ISO…", L"Baixando o arquivo ISO…", L"Загрузка файла ISO…",
     L"ISO ファイルをダウンロード中…",
     L"正在下载 ISO 文件…",
     L"ISO 파일 다운로드 중…",
     L"ISO-bestand downloaden…",
     L"Pobieranie pliku ISO…",
    L"Завантаження файлу ISO…" }},
  {{ L"SHA-256 sağlaması doğrulanıyor…", L"Verifying SHA-256 checksum…", L"SHA-256-Prüfsumme wird verifiziert…",
     L"Vérification de la somme SHA-256…", L"Verificando la suma SHA-256…",
     L"Verifica del checksum SHA-256…", L"Verificando a soma SHA-256…", L"Проверка контрольной суммы SHA-256…",
     L"SHA-256 チェックサムを検証中…",
     L"正在验证 SHA-256 校验和…",
     L"SHA-256 체크섬 확인 중…",
     L"SHA-256-controlesom verifiëren…",
     L"Weryfikacja sumy SHA-256…",
    L"Перевірка контрольної суми SHA-256…" }},
  {{ L"ISO bağlanıyor…", L"Mounting the ISO…", L"ISO wird eingebunden…",
     L"Montage de l'ISO…", L"Montando la ISO…",
     L"Montaggio dell'ISO…", L"Montando a ISO…", L"Подключение ISO…",
     L"ISO をマウント中…",
     L"正在挂载 ISO…",
     L"ISO 마운트 중…",
     L"ISO koppelen…",
     L"Montowanie ISO…",
    L"Підключення ISO…" }},
  {{ L"Önyükleme dosyaları EFI bölümüne kopyalanıyor…", L"Copying boot files to the EFI partition…",
     L"Startdateien werden auf die EFI-Partition kopiert…", L"Copie des fichiers de démarrage vers la partition EFI…",
     L"Copiando archivos de arranque a la partición EFI…", L"Copia dei file di avvio nella partizione EFI…",
     L"Copiando arquivos de inicialização para a partição EFI…", L"Копирование загрузочных файлов в раздел EFI…",
     L"起動ファイルを EFI パーティションにコピー中…",
     L"正在将启动文件复制到 EFI 分区…",
     L"부팅 파일을 EFI 파티션에 복사 중…",
     L"Opstartbestanden naar EFI-partitie kopiëren…",
     L"Kopiowanie plików rozruchu do partycji EFI…",
    L"Копіювання завантажувальних файлів у розділ EFI…" }},
  {{ L"GRUB yapılandırması yazılıyor…", L"Writing GRUB configuration…", L"GRUB-Konfiguration wird geschrieben…",
     L"Écriture de la configuration GRUB…", L"Escribiendo la configuración de GRUB…",
     L"Scrittura della configurazione GRUB…", L"Gravando a configuração do GRUB…", L"Запись конфигурации GRUB…",
     L"GRUB 構成を書き込み中…",
     L"正在写入 GRUB 配置…",
     L"GRUB 구성 작성 중…",
     L"GRUB-configuratie schrijven…",
     L"Zapisywanie konfiguracji GRUB…",
    L"Запис конфігурації GRUB…" }},
  {{ L"Windows Boot Manager'a giriş ekleniyor…", L"Adding entry to Windows Boot Manager…",
     L"Eintrag wird zum Windows Boot Manager hinzugefügt…", L"Ajout de l'entrée au Windows Boot Manager…",
     L"Agregando entrada al Windows Boot Manager…", L"Aggiunta della voce a Windows Boot Manager…",
     L"Adicionando entrada ao Windows Boot Manager…", L"Добавление записи в Windows Boot Manager…",
     L"Windows Boot Manager にエントリを追加中…",
     L"正在向 Windows Boot Manager 添加条目…",
     L"Windows Boot Manager에 항목 추가 중…",
     L"Vermelding toevoegen aan Windows Boot Manager…",
     L"Dodawanie wpisu do Windows Boot Manager…",
    L"Додавання запису до Windows Boot Manager…" }},
  {{ L"Tamamlandı", L"Completed", L"Abgeschlossen", L"Terminé",
     L"Completado", L"Completato", L"Concluído", L"Завершено",
     L"完了",
     L"完成",
     L"완료",
     L"Voltooid",
     L"Zakończono",
    L"Завершено" }},
};

// ================================================================================
//  PAGE: Restart / Error
// ================================================================================
inline const Txt FIN_TITLE = {{
    L"Her şey hazır",
    L"All set",
    L"Alles bereit",
    L"Tout est prêt",
    L"Todo listo",
    L"Tutto pronto",
    L"Tudo pronto",
    L"Всё готово",
     L"すべて完了",
     L"全部就绪",
     L"모두 완료",
     L"Alles klaar",
     L"Wszystko gotowe",
    L"Усе готово" }};

inline const Txt FIN_SUB = {{
    L"Bilgisayarınız yeniden başlatıldığında doğrudan Linux Mint kurulum ortamı açılacak. Windows'a dönmek isterseniz GRUB menüsünden \"Windows Boot Manager\"ı seçmeniz yeterli.",
    L"When your PC restarts, it will boot straight into the Linux Mint installation environment. To return to Windows, just pick \"Windows Boot Manager\" from the GRUB menu.",
    L"Nach dem Neustart startet Ihr PC direkt in die Linux-Mint-Installationsumgebung. Um zu Windows zurückzukehren, wählen Sie im GRUB-Menü \"Windows Boot Manager\".",
    L"Au redémarrage, votre PC ouvrira directement l'environnement d'installation de Linux Mint. Pour revenir à Windows, choisissez \"Windows Boot Manager\" dans le menu GRUB.",
    L"Al reiniciar, su PC arrancará directamente en el entorno de instalación de Linux Mint. Para volver a Windows, elija \"Windows Boot Manager\" en el menú GRUB.",
    L"Al riavvio, il PC aprirà direttamente l'ambiente di installazione di Linux Mint. Per tornare a Windows, scegli \"Windows Boot Manager\" dal menu GRUB.",
    L"Ao reiniciar, seu PC abrirá diretamente o ambiente de instalação do Linux Mint. Para voltar ao Windows, escolha \"Windows Boot Manager\" no menu GRUB.",
    L"После перезагрузки компьютер сразу откроет среду установки Linux Mint. Чтобы вернуться в Windows, выберите \"Windows Boot Manager\" в меню GRUB.",
     L"PC を再起動すると、Linux Mint のインストール環境が直接起動します。Windows に戻るには、GRUB メニューで「Windows Boot Manager」を選択してください。",
     L"重启后，电脑将直接进入 Linux Mint 安装环境。要返回 Windows，只需在 GRUB 菜单中选择“Windows Boot Manager”。",
     L"PC를 다시 시작하면 Linux Mint 설치 환경으로 바로 부팅됩니다. Windows로 돌아가려면 GRUB 메뉴에서 \"Windows Boot Manager\"를 선택하세요.",
     L"Na herstart start uw pc direct op in de Linux Mint-installatieomgeving. Kies \"Windows Boot Manager\" in het GRUB-menu om terug te keren naar Windows.",
     L"Po ponownym uruchomieniu komputer wystartuje bezpośrednio do środowiska instalacji Linux Mint. Aby wrócić do Windows, wybierz „Windows Boot Manager” w menu GRUB.",
    L"Після перезавантаження ПК одразу відкриє середовище встановлення Linux Mint. Щоб повернутися до Windows, виберіть \"Windows Boot Manager\" у меню GRUB." }};

inline const Txt FIN_REVERT = {{
    L"Girdiyi kaldırmak için: C:\\MintInstall\\geri_al.cmd (yönetici olarak)",
    L"To remove the entry: C:\\MintInstall\\geri_al.cmd (run as administrator)",
    L"Zum Entfernen des Eintrags: C:\\MintInstall\\geri_al.cmd (als Administrator)",
    L"Pour supprimer l'entrée : C:\\MintInstall\\geri_al.cmd (en administrateur)",
    L"Para quitar la entrada: C:\\MintInstall\\geri_al.cmd (como administrador)",
    L"Per rimuovere la voce: C:\\MintInstall\\geri_al.cmd (come amministratore)",
    L"Para remover a entrada: C:\\MintInstall\\geri_al.cmd (como administrador)",
    L"Чтобы удалить запись: C:\\MintInstall\\geri_al.cmd (от имени администратора)",
     L"エントリを削除するには: C:\\MintInstall\\geri_al.cmd (管理者として実行)",
     L"要删除条目：C:\\MintInstall\\geri_al.cmd（以管理员身份运行）",
     L"항목을 제거하려면: C:\\MintInstall\\geri_al.cmd (관리자 권한으로 실행)",
     L"Vermelding verwijderen: C:\\MintInstall\\geri_al.cmd (als administrator)",
     L"Aby usunąć wpis: C:\\MintInstall\\geri_al.cmd (jako administrator)",
    L"Щоб видалити запис: C:\\MintInstall\\geri_al.cmd (від імені адміністратора)" }};

inline const Txt FIN_LATER_TXT = {{
    L"Hazır. Dilediğiniz zaman bilgisayarı yeniden başlatabilirsiniz.",
    L"Done. You can restart your PC whenever you like.",
    L"Fertig. Sie können den PC jederzeit neu starten.",
    L"Terminé. Vous pouvez redémarrer votre PC quand vous le souhaitez.",
    L"Listo. Puede reiniciar su PC cuando quiera.",
    L"Fatto. Puoi riavviare il PC quando vuoi.",
    L"Pronto. Você pode reiniciar o PC quando quiser.",
    L"Готово. Перезагрузите компьютер, когда будет удобно.",
     L"準備完了。いつでも PC を再起動できます。",
     L"已就绪。您可以随时重启电脑。",
     L"완료. 언제든지 PC를 다시 시작할 수 있습니다.",
     L"Klaar. U kunt uw pc op elk moment herstarten.",
     L"Gotowe. Możesz zrestartować komputer w dowolnej chwili.",
    L"Готово. Перезавантажте ПК, коли буде зручно." }};

inline const Txt ERR_TITLE = {{
    L"Bir sorun oluştu",
    L"Something went wrong",
    L"Es ist ein Problem aufgetreten",
    L"Un problème est survenu",
    L"Se produjo un problema",
    L"Si è verificato un problema",
    L"Ocorreu um problema",
    L"Возникла проблема",
     L"問題が発生しました",
     L"出现问题",
     L"문제가 발생했습니다",
     L"Er is iets misgegaan",
     L"Wystąpił problem",
    L"Сталася помилка" }};

inline const Txt ERR_LOG_LABEL = {{
    L"Ayrıntılar: ", L"Details: ", L"Details: ", L"Détails : ",
    L"Detalles: ", L"Dettagli: ", L"Detalhes: ", L"Подробности: ",
     L"詳細: ",
     L"详细信息: ",
     L"세부 정보: ",
     L"Details: ",
     L"Szczegóły: ",
    L"Подробиці: " }};

inline const wchar_t* ERR_LOG_PATH = L"C:\\MintInstall\\assistant.log";   // path — not translated

// ================================================================================
//  BUTTONS
// ================================================================================
inline const Txt BTN_NEXT = {{
    L"İleri", L"Next", L"Weiter", L"Suivant", L"Siguiente", L"Avanti", L"Avançar", L"Далее",
     L"次へ",
     L"下一步",
     L"다음",
     L"Volgende",
     L"Dalej",
    L"Далі" }};
inline const Txt BTN_BACK = {{
    L"Geri", L"Back", L"Zurück", L"Retour", L"Atrás", L"Indietro", L"Voltar", L"Назад",
     L"戻る",
     L"上一步",
     L"뒤로",
     L"Terug",
     L"Wstecz",
    L"Назад" }};
inline const Txt BTN_INSTALL = {{
    L"Yükle", L"Install", L"Installieren", L"Installer", L"Instalar", L"Installa", L"Instalar", L"Установить",
     L"インストール",
     L"安装",
     L"설치",
     L"Installeren",
     L"Zainstaluj",
    L"Встановлення" }};
inline const Txt BTN_CANCEL = {{
    L"İptal", L"Cancel", L"Abbrechen", L"Annuler", L"Cancelar", L"Annulla", L"Cancelar", L"Отмена",
     L"キャンセル",
     L"取消",
     L"취소",
     L"Annuleren",
     L"Anuluj",
    L"Скасувати" }};
inline const Txt BTN_CANCELLING = {{
    L"İptal ediliyor…", L"Cancelling…", L"Wird abgebrochen…", L"Annulation…",
    L"Cancelando…", L"Annullamento…", L"Cancelando…", L"Отмена…",
     L"キャンセル中…",
     L"正在取消…",
     L"취소 중…",
     L"Annuleren…",
     L"Anulowanie…",
    L"Скасування…" }};
inline const Txt BTN_RESTART = {{
    L"Şimdi yeniden başlat", L"Restart now", L"Jetzt neu starten", L"Redémarrer maintenant",
    L"Reiniciar ahora", L"Riavvia ora", L"Reiniciar agora", L"Перезагрузить сейчас",
     L"今すぐ再起動",
     L"立即重启",
     L"지금 다시 시작",
     L"Nu herstarten",
     L"Uruchom ponownie",
    L"Перезавантажити зараз" }};
inline const Txt BTN_LATER = {{
    L"Daha sonra", L"Later", L"Später", L"Plus tard", L"Más tarde", L"Più tardi", L"Mais tarde", L"Позже",
     L"後で",
     L"稍后",
     L"나중에",
     L"Later",
     L"Później",
    L"Пізніше" }};
inline const Txt BTN_CLOSE = {{
    L"Kapat", L"Close", L"Schließen", L"Fermer", L"Cerrar", L"Chiudi", L"Fechar", L"Закрыть",
     L"閉じる",
     L"关闭",
     L"닫기",
     L"Sluiten",
     L"Zamknij",
    L"Закрити" }};

// ================================================================================
//  UYUMLULUK RAPORU (HTML) + HATA MESAJLARI
// ================================================================================
inline const Txt RPT_TITLE_FMT = {{                     // printf: %s = version
    L"Linux Mint %s — Uyumluluk Raporu",
    L"Linux Mint %s — Compatibility Report",
    L"Linux Mint %s — Kompatibilitätsbericht",
    L"Linux Mint %s — Rapport de compatibilité",
    L"Linux Mint %s — Informe de compatibilidad",
    L"Linux Mint %s — Rapporto di compatibilità",
    L"Linux Mint %s — Relatório de compatibilidade",
    L"Linux Mint %s — Отчёт о совместимости",
    L"Linux Mint %s — 互換性レポート",
    L"Linux Mint %s — 兼容性报告",
    L"Linux Mint %s — 호환성 보고서",
    L"Linux Mint %s — Compatibiliteitsrapport",
    L"Linux Mint %s — Raport zgodności",
    L"Linux Mint %s — Звіт про сумісність" }};

inline const Txt RPT_SUMMARY_FMT = {{                   // printf: %d %d %d
    L"%d uygulama uyumlu &nbsp;·&nbsp; %d öğe gözden geçirilmeli &nbsp;·&nbsp; %d öğe değerlendirilmedi",
    L"%d apps compatible &nbsp;·&nbsp; %d items to review &nbsp;·&nbsp; %d items not evaluated",
    L"%d Apps kompatibel &nbsp;·&nbsp; %d Elemente zu prüfen &nbsp;·&nbsp; %d Elemente nicht bewertet",
    L"%d applications compatibles &nbsp;·&nbsp; %d éléments à examiner &nbsp;·&nbsp; %d éléments non évalués",
    L"%d aplicaciones compatibles &nbsp;·&nbsp; %d elementos por revisar &nbsp;·&nbsp; %d elementos no evaluados",
    L"%d app compatibili &nbsp;·&nbsp; %d elementi da esaminare &nbsp;·&nbsp; %d elementi non valutati",
    L"%d aplicativos compatíveis &nbsp;·&nbsp; %d itens para revisar &nbsp;·&nbsp; %d itens não avaliados",
    L"Совместимо приложений: %d &nbsp;·&nbsp; на проверку: %d &nbsp;·&nbsp; не оценено: %d",
    L"%d 個のアプリが互換 &nbsp;·&nbsp; %d 個の項目を要確認 &nbsp;·&nbsp; %d 個の項目は未評価",
    L"%d 个应用兼容 &nbsp;·&nbsp; %d 个项目待检查 &nbsp;·&nbsp; %d 个项目未评估",
    L"호환 앱 %d개 &nbsp;·&nbsp; 검토 필요 %d개 &nbsp;·&nbsp; 미평가 %d개",
    L"%d apps compatibel &nbsp;·&nbsp; %d items te controleren &nbsp;·&nbsp; %d items niet beoordeeld",
    L"%d zgodnych aplikacji &nbsp;·&nbsp; %d elementów do sprawdzenia &nbsp;·&nbsp; %d nieocenionych",
    L"Сумісних застосунків: %d &nbsp;·&nbsp; на перевірку: %d &nbsp;·&nbsp; не оцінено: %d" }};

inline const Txt RPT_COL_APP = {{
    L"Uygulama", L"Application", L"Anwendung", L"Application", L"Aplicación",
    L"Applicazione", L"Aplicativo", L"Приложение", L"アプリ", L"应用",
    L"애플리케이션", L"Toepassing", L"Aplikacja", L"Застосунок" }};

inline const Txt RPT_COL_STATUS = {{
    L"Durum", L"Status", L"Status", L"État", L"Estado",
    L"Stato", L"Status", L"Статус", L"状態", L"状态",
    L"상태", L"Status", L"Stan", L"Стан" }};

inline const Txt RPT_COL_ONMINT = {{
    L"Linux Mint'te", L"On Linux Mint", L"Unter Linux Mint", L"Sur Linux Mint",
    L"En Linux Mint", L"Su Linux Mint", L"No Linux Mint", L"В Linux Mint",
    L"Linux Mint では", L"在 Linux Mint 上", L"Linux Mint에서", L"Op Linux Mint",
    L"W Linux Mint", L"У Linux Mint" }};

// Uyumluluk durum etiketleri
inline const Txt CST_NATIVE = {{
    L"Uyumlu", L"Compatible", L"Kompatibel", L"Compatible", L"Compatible",
    L"Compatibile", L"Compatível", L"Совместимо", L"互換あり", L"兼容",
    L"호환됨", L"Compatibel", L"Zgodne", L"Сумісно" }};

inline const Txt CST_ALT = {{
    L"Alternatif önerilir", L"Alternative suggested", L"Alternative empfohlen",
    L"Alternative suggérée", L"Alternativa sugerida", L"Alternativa consigliata",
    L"Alternativa sugerida", L"Рекомендуется альтернатива", L"代替を推奨", L"建议替代",
    L"대체 권장", L"Alternatief aanbevolen", L"Zalecana alternatywa", L"Рекомендовано альтернативу" }};

inline const Txt CST_CAUTION = {{
    L"Dikkat", L"Caution", L"Achtung", L"Attention", L"Precaución",
    L"Attenzione", L"Atenção", L"Внимание", L"注意", L"注意",
    L"주의", L"Let op", L"Uwaga", L"Увага" }};

inline const Txt CST_UNKNOWN = {{
    L"Değerlendirilmedi", L"Not evaluated", L"Nicht bewertet", L"Non évalué",
    L"No evaluado", L"Non valutato", L"Não avaliado", L"Не оценено", L"未評価", L"未评估",
    L"미평가", L"Niet beoordeeld", L"Nieoceniono", L"Не оцінено" }};

// Error messages (install/download)
inline const Txt ERR_CANCELLED = {{
    L"İptal edildi.", L"Cancelled.", L"Abgebrochen.", L"Annulé.", L"Cancelado.",
    L"Annullato.", L"Cancelado.", L"Отменено.", L"キャンセルされました。", L"已取消。",
    L"취소되었습니다.", L"Geannuleerd.", L"Anulowano.", L"Скасовано." }};

inline const Txt ERR_SHA_FMT = {{
    L"Sağlama dosyası hiçbir depodan alınamadı.\nAğ bağlantınızı denetleyip yeniden deneyin.",
    L"The checksum file could not be fetched from any mirror.\nCheck your network connection and try again.",
    L"Die Prüfsummendatei konnte von keinem Spiegel abgerufen werden.\nPrüfen Sie Ihre Netzwerkverbindung und versuchen Sie es erneut.",
    L"Le fichier de somme de contrôle n'a pu être récupéré depuis aucun miroir.\nVérifiez votre connexion réseau et réessayez.",
    L"No se pudo obtener el archivo de suma de comprobación de ningún servidor.\nCompruebe su conexión de red e inténtelo de nuevo.",
    L"Impossibile scaricare il file di checksum da alcun mirror.\nControlla la connessione di rete e riprova.",
    L"Não foi possível obter o arquivo de soma de verificação de nenhum espelho.\nVerifique sua conexão de rede e tente novamente.",
    L"Не удалось загрузить файл контрольной суммы ни с одного зеркала.\nПроверьте подключение к сети и повторите попытку.",
    L"チェックサムファイルをどのミラーからも取得できませんでした。\nネットワーク接続を確認して再試行してください。",
    L"无法从任何镜像获取校验和文件。\n请检查网络连接后重试。",
    L"어떤 미러에서도 체크섬 파일을 가져올 수 없습니다.\n네트워크 연결을 확인한 후 다시 시도하세요.",
    L"Het controlesombestand kon van geen enkele mirror worden opgehaald.\nControleer uw netwerkverbinding en probeer opnieuw.",
    L"Nie udało się pobrać pliku sumy kontrolnej z żadnego serwera lustrzanego.\nSprawdź połączenie sieciowe i spróbuj ponownie.",
    L"Не вдалося завантажити файл контрольної суми з жодного дзеркала.\nПеревірте підключення до мережі та повторіть спробу." }};

inline const Txt ERR_ISO_FMT = {{                       // printf: %s = son hata
    L"ISO hiçbir depodan indirilemedi.\nSon hata: %s",
    L"The ISO could not be downloaded from any mirror.\nLast error: %s",
    L"Die ISO konnte von keinem Spiegel heruntergeladen werden.\nLetzter Fehler: %s",
    L"L'ISO n'a pu être téléchargée depuis aucun miroir.\nDernière erreur : %s",
    L"No se pudo descargar la ISO de ningún servidor.\nÚltimo error: %s",
    L"Impossibile scaricare l'ISO da alcun mirror.\nUltimo errore: %s",
    L"Não foi possível baixar a ISO de nenhum espelho.\nÚltimo erro: %s",
    L"Не удалось загрузить ISO ни с одного зеркала.\nПоследняя ошибка: %s",
    L"ISO をどのミラーからもダウンロードできませんでした。\n最後のエラー: %s",
    L"无法从任何镜像下载 ISO。\n最后的错误：%s",
    L"어떤 미러에서도 ISO를 다운로드할 수 없습니다.\n마지막 오류: %s",
    L"De ISO kon van geen enkele mirror worden gedownload.\nLaatste fout: %s",
    L"Nie udało się pobrać ISO z żadnego serwera lustrzanego.\nOstatni błąd: %s",
    L"Не вдалося завантажити ISO з жодного дзеркала.\nОстання помилка: %s" }};

inline const Txt ERR_SHA_COMPUTE_FMT = {{               // printf: %s = detail
    L"Sağlama hesaplanamadı.\n%s",
    L"Could not compute the checksum.\n%s",
    L"Die Prüfsumme konnte nicht berechnet werden.\n%s",
    L"Impossible de calculer la somme de contrôle.\n%s",
    L"No se pudo calcular la suma de comprobación.\n%s",
    L"Impossibile calcolare il checksum.\n%s",
    L"Não foi possível calcular a soma de verificação.\n%s",
    L"Не удалось вычислить контрольную сумму.\n%s",
    L"チェックサムを計算できませんでした。\n%s",
    L"无法计算校验和。\n%s",
    L"체크섬을 계산할 수 없습니다.\n%s",
    L"Kon de controlesom niet berekenen.\n%s",
    L"Nie udało się obliczyć sumy kontrolnej.\n%s",
    L"Не вдалося обчислити контрольну суму.\n%s" }};

inline const Txt ERR_SHA_MISMATCH = {{
    L"SHA-256 doğrulaması BAŞARISIZ.\nBozuk indirme silindi; lütfen yeniden deneyin.",
    L"SHA-256 verification FAILED.\nThe corrupt download was deleted; please try again.",
    L"SHA-256-Überprüfung FEHLGESCHLAGEN.\nDer beschädigte Download wurde gelöscht; bitte erneut versuchen.",
    L"Échec de la vérification SHA-256.\nLe téléchargement corrompu a été supprimé ; veuillez réessayer.",
    L"Falló la verificación SHA-256.\nLa descarga dañada se eliminó; inténtelo de nuevo.",
    L"Verifica SHA-256 NON RIUSCITA.\nIl download corrotto è stato eliminato; riprova.",
    L"Falha na verificação SHA-256.\nO download corrompido foi excluído; tente novamente.",
    L"Проверка SHA-256 НЕ ПРОЙДЕНА.\nПовреждённая загрузка удалена; повторите попытку.",
    L"SHA-256 検証に失敗しました。\n破損したダウンロードを削除しました。もう一度お試しください。",
    L"SHA-256 校验失败。\n已删除损坏的下载；请重试。",
    L"SHA-256 검증에 실패했습니다.\n손상된 다운로드를 삭제했습니다. 다시 시도하세요.",
    L"SHA-256-verificatie MISLUKT.\nDe beschadigde download is verwijderd; probeer opnieuw.",
    L"Weryfikacja SHA-256 NIE POWIODŁA SIĘ.\nUszkodzone pobranie usunięto; spróbuj ponownie.",
    L"Перевірка SHA-256 НЕ ПРОЙДЕНА.\nПошкоджене завантаження видалено; повторіть спробу." }};

} // namespace str

// ============================================================================
//  Compatibility note translation (the AppCompat DB contains Turkish sentences;
//  WITHOUT touching the DB, it translates known Turkish patterns to the selected
//  language at display time. If there's no match (e.g. product names like "GIMP",
//  "LibreOffice / OnlyOffice") the note is left as-is — universal, no translation needed.)
// ============================================================================
namespace str {

// Turkish pattern -> 14-language translation. Only sentence-bearing notes here.
struct NoteTr { const wchar_t* tr; Txt tx; };

inline const NoteTr NOTE_MAP[] = {
  { L"Yerel Linux sürümü mevcut", {{
      L"Yerel Linux sürümü mevcut", L"Native Linux version available", L"Native Linux-Version verfügbar",
      L"Version Linux native disponible", L"Versión nativa de Linux disponible", L"Versione Linux nativa disponibile",
      L"Versão nativa para Linux disponível", L"Есть нативная версия для Linux", L"ネイティブの Linux 版があります",
      L"提供原生 Linux 版本", L"네이티브 Linux 버전 제공", L"Native Linux-versie beschikbaar",
      L"Dostępna natywna wersja dla Linuksa", L"Доступна нативна версія для Linux" }} },
  { L"Yerel Linux istemcisi mevcut", {{
      L"Yerel Linux istemcisi mevcut", L"Native Linux client available", L"Nativer Linux-Client verfügbar",
      L"Client Linux natif disponible", L"Cliente nativo de Linux disponible", L"Client Linux nativo disponibile",
      L"Cliente nativo para Linux disponível", L"Есть нативный клиент для Linux", L"ネイティブの Linux クライアントがあります",
      L"提供原生 Linux 客户端", L"네이티브 Linux 클라이언트 제공", L"Native Linux-client beschikbaar",
      L"Dostępny natywny klient dla Linuksa", L"Доступний нативний клієнт для Linux" }} },
  { L"Linux'ta standarttır", {{
      L"Linux'ta standarttır", L"Standard on Linux", L"Standard unter Linux",
      L"Standard sous Linux", L"Estándar en Linux", L"Standard su Linux",
      L"Padrão no Linux", L"Стандарт в Linux", L"Linux では標準",
      L"在 Linux 上是标准组件", L"Linux에서 표준", L"Standaard op Linux",
      L"Standard w systemie Linux", L"Стандарт у Linux" }} },
  { L"Mint ile birlikte gelir", {{
      L"Mint ile birlikte gelir", L"Comes with Mint", L"In Mint enthalten",
      L"Fourni avec Mint", L"Incluido con Mint", L"Incluso in Mint",
      L"Vem com o Mint", L"Поставляется с Mint", L"Mint に付属",
      L"随 Mint 一起提供", L"Mint에 기본 포함", L"Wordt met Mint meegeleverd",
      L"Dołączony do Mint", L"Постачається з Mint" }} },
  { L"Yerel Linux istemcisi var; binlerce oyun Proton ile çalışır", {{
      L"Yerel Linux istemcisi var; binlerce oyun Proton ile çalışır",
      L"Native Linux client; thousands of games run via Proton",
      L"Nativer Linux-Client; Tausende Spiele laufen über Proton",
      L"Client Linux natif ; des milliers de jeux via Proton",
      L"Cliente nativo de Linux; miles de juegos con Proton",
      L"Client Linux nativo; migliaia di giochi con Proton",
      L"Cliente nativo para Linux; milhares de jogos via Proton",
      L"Нативный клиент; тысячи игр работают через Proton",
      L"ネイティブ Linux クライアント。Proton で数千のゲームが動作",
      L"原生 Linux 客户端；数千款游戏可通过 Proton 运行",
      L"네이티브 Linux 클라이언트; Proton으로 수천 개 게임 실행",
      L"Native Linux-client; duizenden games via Proton",
      L"Natywny klient; tysiące gier działa przez Proton",
      L"Нативний клієнт; тисячі ігор працюють через Proton" }} },
  { L"Anti-cheat Linux'a kapalı; çalışmaz", {{
      L"Anti-cheat Linux'a kapalı; çalışmaz",
      L"Anti-cheat blocks Linux; does not run",
      L"Anti-Cheat blockiert Linux; läuft nicht",
      L"L'anti-triche bloque Linux ; ne fonctionne pas",
      L"El anti-trampas bloquea Linux; no funciona",
      L"L'anti-cheat blocca Linux; non funziona",
      L"O anti-trapaça bloqueia o Linux; não funciona",
      L"Античит блокирует Linux; не запускается",
      L"アンチチートが Linux を非対応。動作しません",
      L"反作弊阻止 Linux；无法运行",
      L"안티치트가 Linux를 차단; 실행 안 됨",
      L"Anti-cheat blokkeert Linux; werkt niet",
      L"Anti-cheat blokuje Linux; nie działa",
      L"Античит блокує Linux; не працює" }} },
  { L"Anti-cheat nedeniyle Linux'ta ÇALIŞMAZ", {{
      L"Anti-cheat nedeniyle Linux'ta ÇALIŞMAZ",
      L"DOES NOT run on Linux due to anti-cheat",
      L"Läuft wegen Anti-Cheat NICHT unter Linux",
      L"NE fonctionne PAS sous Linux (anti-triche)",
      L"NO funciona en Linux por el anti-trampas",
      L"NON funziona su Linux per l'anti-cheat",
      L"NÃO funciona no Linux devido ao anti-trapaça",
      L"НЕ работает на Linux из-за античита",
      L"アンチチートのため Linux では動作しません",
      L"因反作弊，无法在 Linux 上运行",
      L"안티치트로 인해 Linux에서 실행 안 됨",
      L"Werkt NIET op Linux door anti-cheat",
      L"NIE działa na Linuksie z powodu anti-cheat",
      L"НЕ працює на Linux через античит" }} },
  { L"Anti-cheat nedeniyle çoğu sürüm Linux'ta çalışmaz", {{
      L"Anti-cheat nedeniyle çoğu sürüm Linux'ta çalışmaz",
      L"Most versions won't run on Linux (anti-cheat)",
      L"Die meisten Versionen laufen nicht unter Linux (Anti-Cheat)",
      L"La plupart des versions ne fonctionnent pas sous Linux (anti-triche)",
      L"La mayoría de versiones no funcionan en Linux (anti-trampas)",
      L"La maggior parte delle versioni non funziona su Linux (anti-cheat)",
      L"A maioria das versões não funciona no Linux (anti-trapaça)",
      L"Большинство версий не работают на Linux (античит)",
      L"アンチチートのため多くのバージョンは Linux で動作しません",
      L"因反作弊，多数版本无法在 Linux 上运行",
      L"안티치트로 대부분 버전이 Linux에서 실행 안 됨",
      L"Meeste versies werken niet op Linux (anti-cheat)",
      L"Większość wersji nie działa na Linuksie (anti-cheat)",
      L"Більшість версій не працюють на Linux (античит)" }} },
  { L"Arşiv Yöneticisi (yerleşik)", {{
      L"Arşiv Yöneticisi (yerleşik)",
      L"Archive Manager (built-in)",
      L"Archivverwaltung (integriert)",
      L"Gestionnaire d'archives (intégré)",
      L"Gestor de archivos (integrado)",
      L"Gestore archivi (integrato)",
      L"Gerenciador de arquivos (integrado)",
      L"Менеджер архивов (встроенный)",
      L"アーカイブマネージャー（内蔵）",
      L"归档管理器（内置）",
      L"압축 관리자(기본 제공)",
      L"Archiefbeheer (ingebouwd)",
      L"Menedżer archiwów (wbudowany)",
      L"Менеджер архівів (вбудований)" }} },
  { L"Arşiv Yöneticisi / PeaZip", {{
      L"Arşiv Yöneticisi / PeaZip",
      L"Archive Manager / PeaZip",
      L"Archivverwaltung / PeaZip",
      L"Gestionnaire d'archives / PeaZip",
      L"Gestor de archivos / PeaZip",
      L"Gestore archivi / PeaZip",
      L"Gerenciador de arquivos / PeaZip",
      L"Менеджер архивов / PeaZip",
      L"アーカイブマネージャー / PeaZip",
      L"归档管理器 / PeaZip",
      L"압축 관리자 / PeaZip",
      L"Archiefbeheer / PeaZip",
      L"Menedżer archiwów / PeaZip",
      L"Менеджер архівів / PeaZip" }} },
  { L"Baobab (Disk Kullanımı) / QDirStat", {{
      L"Baobab (Disk Kullanımı) / QDirStat",
      L"Baobab (Disk Usage) / QDirStat",
      L"Baobab (Festplattenbelegung) / QDirStat",
      L"Baobab (Utilisation du disque) / QDirStat",
      L"Baobab (Uso de disco) / QDirStat",
      L"Baobab (Uso disco) / QDirStat",
      L"Baobab (Uso de disco) / QDirStat",
      L"Baobab (Использование диска) / QDirStat",
      L"Baobab（ディスク使用量）/ QDirStat",
      L"Baobab（磁盘使用）/ QDirStat",
      L"Baobab(디스크 사용량) / QDirStat",
      L"Baobab (Schijfgebruik) / QDirStat",
      L"Baobab (Wykorzystanie dysku) / QDirStat",
      L"Baobab (Використання диска) / QDirStat" }} },
  { L"BattlEye nedeniyle çoğunlukla çalışmaz", {{
      L"BattlEye nedeniyle çoğunlukla çalışmaz",
      L"Usually won't run due to BattlEye",
      L"Läuft wegen BattlEye meist nicht",
      L"Ne fonctionne généralement pas (BattlEye)",
      L"Normalmente no funciona por BattlEye",
      L"Di solito non funziona per BattlEye",
      L"Geralmente não funciona devido ao BattlEye",
      L"Обычно не работает из-за BattlEye",
      L"BattlEye のため通常動作しません",
      L"因 BattlEye，通常无法运行",
      L"BattlEye로 대개 실행 안 됨",
      L"Werkt meestal niet door BattlEye",
      L"Zwykle nie działa z powodu BattlEye",
      L"Зазвичай не працює через BattlEye" }} },
  { L"BattlEye/anti-cheat; genelde çalışmaz", {{
      L"BattlEye/anti-cheat; genelde çalışmaz",
      L"BattlEye/anti-cheat; usually won't run",
      L"BattlEye/Anti-Cheat; läuft meist nicht",
      L"BattlEye/anti-triche ; ne fonctionne généralement pas",
      L"BattlEye/anti-trampas; normalmente no funciona",
      L"BattlEye/anti-cheat; di solito non funziona",
      L"BattlEye/anti-trapaça; geralmente não funciona",
      L"BattlEye/античит; обычно не работает",
      L"BattlEye/アンチチート。通常動作しません",
      L"BattlEye/反作弊；通常无法运行",
      L"BattlEye/안티치트; 대개 실행 안 됨",
      L"BattlEye/anti-cheat; werkt meestal niet",
      L"BattlEye/anti-cheat; zwykle nie działa",
      L"BattlEye/античит; зазвичай не працює" }} },
  { L"Belge Görüntüleyici (Xreader) / Okular", {{
      L"Belge Görüntüleyici (Xreader) / Okular",
      L"Document Viewer (Xreader) / Okular",
      L"Dokumentenbetrachter (Xreader) / Okular",
      L"Visionneuse de documents (Xreader) / Okular",
      L"Visor de documentos (Xreader) / Okular",
      L"Visualizzatore documenti (Xreader) / Okular",
      L"Visualizador de documentos (Xreader) / Okular",
      L"Просмотр документов (Xreader) / Okular",
      L"ドキュメントビューアー（Xreader）/ Okular",
      L"文档查看器（Xreader）/ Okular",
      L"문서 뷰어(Xreader) / Okular",
      L"Documentviewer (Xreader) / Okular",
      L"Przeglądarka dokumentów (Xreader) / Okular",
      L"Перегляд документів (Xreader) / Okular" }} },
  { L"Docker Engine Linux'ta yereldir (daha hızlı)", {{
      L"Docker Engine Linux'ta yereldir (daha hızlı)",
      L"Docker Engine is native on Linux (faster)",
      L"Docker Engine ist unter Linux nativ (schneller)",
      L"Docker Engine est natif sous Linux (plus rapide)",
      L"Docker Engine es nativo en Linux (más rápido)",
      L"Docker Engine è nativo su Linux (più veloce)",
      L"O Docker Engine é nativo no Linux (mais rápido)",
      L"Docker Engine нативен в Linux (быстрее)",
      L"Docker Engine は Linux でネイティブ（高速）",
      L"Docker Engine 在 Linux 上原生运行（更快）",
      L"Docker Engine은 Linux에서 네이티브(더 빠름)",
      L"Docker Engine is native op Linux (sneller)",
      L"Docker Engine jest natywny na Linuksie (szybszy)",
      L"Docker Engine нативний на Linux (швидше)" }} },
  { L"EAC değişkenliği; genelde Linux'ta çalışmaz", {{
      L"EAC değişkenliği; genelde Linux'ta çalışmaz",
      L"EAC varies; usually won't run on Linux",
      L"EAC variiert; läuft meist nicht unter Linux",
      L"EAC variable ; ne fonctionne généralement pas sous Linux",
      L"EAC variable; normalmente no funciona en Linux",
      L"EAC variabile; di solito non funziona su Linux",
      L"EAC varia; geralmente não funciona no Linux",
      L"EAC по-разному; обычно не работает на Linux",
      L"EAC は不確定。通常 Linux で動作しません",
      L"EAC 不确定；通常无法在 Linux 上运行",
      L"EAC 가변적; 대개 Linux에서 실행 안 됨",
      L"EAC wisselt; werkt meestal niet op Linux",
      L"EAC bywa różnie; zwykle nie działa na Linuksie",
      L"EAC по-різному; зазвичай не працює на Linux" }} },
  { L"Easy Anti-Cheat Linux'a kapalı; çalışmaz", {{
      L"Easy Anti-Cheat Linux'a kapalı; çalışmaz",
      L"Easy Anti-Cheat blocks Linux; does not run",
      L"Easy Anti-Cheat blockiert Linux; läuft nicht",
      L"Easy Anti-Cheat bloque Linux ; ne fonctionne pas",
      L"Easy Anti-Cheat bloquea Linux; no funciona",
      L"Easy Anti-Cheat blocca Linux; non funziona",
      L"O Easy Anti-Cheat bloqueia o Linux; não funciona",
      L"Easy Anti-Cheat блокирует Linux; не запускается",
      L"Easy Anti-Cheat が Linux 非対応。動作しません",
      L"Easy Anti-Cheat 阻止 Linux；无法运行",
      L"Easy Anti-Cheat가 Linux 차단; 실행 안 됨",
      L"Easy Anti-Cheat blokkeert Linux; werkt niet",
      L"Easy Anti-Cheat blokuje Linux; nie działa",
      L"Easy Anti-Cheat блокує Linux; не працює" }} },
  { L"GTA V mod istemcisi; Proton ile kısmi", {{
      L"GTA V mod istemcisi; Proton ile kısmi",
      L"GTA V mod client; partial via Proton",
      L"GTA-V-Mod-Client; teilweise über Proton",
      L"Client mod GTA V ; partiel via Proton",
      L"Cliente mod de GTA V; parcial con Proton",
      L"Client mod GTA V; parziale con Proton",
      L"Cliente mod de GTA V; parcial via Proton",
      L"Мод-клиент GTA V; частично через Proton",
      L"GTA V の MOD クライアント。Proton で部分的",
      L"GTA V 模组客户端；通过 Proton 部分可用",
      L"GTA V 모드 클라이언트; Proton으로 부분 지원",
      L"GTA V-modclient; deels via Proton",
      L"Klient modów GTA V; częściowo przez Proton",
      L"Мод-клієнт GTA V; частково через Proton" }} },
  { L"Gece Işığı (yerleşik) / Redshift", {{
      L"Gece Işığı (yerleşik) / Redshift",
      L"Night Light (built-in) / Redshift",
      L"Nachtlicht (integriert) / Redshift",
      L"Éclairage nocturne (intégré) / Redshift",
      L"Luz nocturna (integrada) / Redshift",
      L"Luce notturna (integrata) / Redshift",
      L"Luz noturna (integrada) / Redshift",
      L"Ночной свет (встроенный) / Redshift",
      L"夜間モード（内蔵）/ Redshift",
      L"夜间模式（内置）/ Redshift",
      L"야간 모드(기본 제공) / Redshift",
      L"Nachtlicht (ingebouwd) / Redshift",
      L"Tryb nocny (wbudowany) / Redshift",
      L"Нічне світло (вбудоване) / Redshift" }} },
  { L"Lutris ile çalışır; bazı oyunlar Proton gerektirir", {{
      L"Lutris ile çalışır; bazı oyunlar Proton gerektirir",
      L"Works via Lutris; some games need Proton",
      L"Läuft über Lutris; einige Spiele brauchen Proton",
      L"Fonctionne via Lutris ; certains jeux nécessitent Proton",
      L"Funciona con Lutris; algunos juegos requieren Proton",
      L"Funziona con Lutris; alcuni giochi richiedono Proton",
      L"Funciona via Lutris; alguns jogos exigem Proton",
      L"Работает через Lutris; некоторым играм нужен Proton",
      L"Lutris で動作。一部のゲームは Proton が必要",
      L"通过 Lutris 运行；部分游戏需要 Proton",
      L"Lutris로 작동; 일부 게임은 Proton 필요",
      L"Werkt via Lutris; sommige games vereisen Proton",
      L"Działa przez Lutris; niektóre gry wymagają Proton",
      L"Працює через Lutris; деяким іграм потрібен Proton" }} },
  { L"Lutris/Proton ile kurulabilir; oyun bazında değişir", {{
      L"Lutris/Proton ile kurulabilir; oyun bazında değişir",
      L"Installable via Lutris/Proton; varies per game",
      L"Über Lutris/Proton installierbar; je nach Spiel",
      L"Installable via Lutris/Proton ; selon le jeu",
      L"Instalable con Lutris/Proton; según el juego",
      L"Installabile con Lutris/Proton; dipende dal gioco",
      L"Instalável via Lutris/Proton; varia por jogo",
      L"Устанавливается через Lutris/Proton; зависит от игры",
      L"Lutris/Proton で導入可能。ゲームによる",
      L"可通过 Lutris/Proton 安装；因游戏而异",
      L"Lutris/Proton으로 설치 가능; 게임마다 다름",
      L"Installeerbaar via Lutris/Proton; per game verschillend",
      L"Instalowalne przez Lutris/Proton; zależnie od gry",
      L"Встановлюється через Lutris/Proton; залежить від гри" }} },
  { L"Lutris/Proton ile kısmi; GTA Online anti-cheat sorunlu", {{
      L"Lutris/Proton ile kısmi; GTA Online anti-cheat sorunlu",
      L"Partial via Lutris/Proton; GTA Online anti-cheat is problematic",
      L"Teilweise über Lutris/Proton; GTA-Online-Anti-Cheat problematisch",
      L"Partiel via Lutris/Proton ; anti-triche GTA Online problématique",
      L"Parcial con Lutris/Proton; el anti-trampas de GTA Online da problemas",
      L"Parziale con Lutris/Proton; anti-cheat di GTA Online problematico",
      L"Parcial via Lutris/Proton; anti-trapaça do GTA Online é problemático",
      L"Частично через Lutris/Proton; античит GTA Online проблемный",
      L"Lutris/Proton で部分的。GTA Online のアンチチートに問題",
      L"通过 Lutris/Proton 部分可用；GTA Online 反作弊有问题",
      L"Lutris/Proton으로 부분 지원; GTA Online 안티치트 문제",
      L"Deels via Lutris/Proton; GTA Online-anti-cheat is problematisch",
      L"Częściowo przez Lutris/Proton; anti-cheat GTA Online sprawia problemy",
      L"Частково через Lutris/Proton; античит GTA Online проблемний" }} },
  { L"Lutris/Proton ile kısmi; anti-cheat'li oyunlar sorunlu", {{
      L"Lutris/Proton ile kısmi; anti-cheat'li oyunlar sorunlu",
      L"Partial via Lutris/Proton; anti-cheat games are problematic",
      L"Teilweise über Lutris/Proton; Anti-Cheat-Spiele problematisch",
      L"Partiel via Lutris/Proton ; jeux avec anti-triche problématiques",
      L"Parcial con Lutris/Proton; los juegos con anti-trampas dan problemas",
      L"Parziale con Lutris/Proton; giochi con anti-cheat problematici",
      L"Parcial via Lutris/Proton; jogos com anti-trapaça são problemáticos",
      L"Частично через Lutris/Proton; игры с античитом проблемны",
      L"Lutris/Proton で部分的。アンチチート付きゲームに問題",
      L"通过 Lutris/Proton 部分可用；带反作弊的游戏有问题",
      L"Lutris/Proton으로 부분 지원; 안티치트 게임은 문제",
      L"Deels via Lutris/Proton; anti-cheatgames zijn problematisch",
      L"Częściowo przez Lutris/Proton; gry z anti-cheat sprawiają problemy",
      L"Частково через Lutris/Proton; ігри з античитом проблемні" }} },
  { L"NVIDIA Ayarları / GreenWithEnvy", {{
      L"NVIDIA Ayarları / GreenWithEnvy",
      L"NVIDIA Settings / GreenWithEnvy",
      L"NVIDIA-Einstellungen / GreenWithEnvy",
      L"Paramètres NVIDIA / GreenWithEnvy",
      L"Configuración NVIDIA / GreenWithEnvy",
      L"Impostazioni NVIDIA / GreenWithEnvy",
      L"Configurações NVIDIA / GreenWithEnvy",
      L"Настройки NVIDIA / GreenWithEnvy",
      L"NVIDIA 設定 / GreenWithEnvy",
      L"NVIDIA 设置 / GreenWithEnvy",
      L"NVIDIA 설정 / GreenWithEnvy",
      L"NVIDIA-instellingen / GreenWithEnvy",
      L"Ustawienia NVIDIA / GreenWithEnvy",
      L"Налаштування NVIDIA / GreenWithEnvy" }} },
  { L"Oyun bazında değişir; bazıları Proton'da çalışır", {{
      L"Oyun bazında değişir; bazıları Proton'da çalışır",
      L"Varies per game; some run via Proton",
      L"Je nach Spiel; einige laufen über Proton",
      L"Selon le jeu ; certains via Proton",
      L"Según el juego; algunos con Proton",
      L"Dipende dal gioco; alcuni con Proton",
      L"Varia por jogo; alguns via Proton",
      L"Зависит от игры; некоторые через Proton",
      L"ゲームによる。一部は Proton で動作",
      L"因游戏而异；部分可通过 Proton 运行",
      L"게임마다 다름; 일부는 Proton으로 작동",
      L"Per game verschillend; sommige via Proton",
      L"Zależnie od gry; niektóre przez Proton",
      L"Залежить від гри; деякі через Proton" }} },
  { L"Oyun bazında; bazıları Proton'da etkin, bazıları kapalı", {{
      L"Oyun bazında; bazıları Proton'da etkin, bazıları kapalı",
      L"Per game; some enabled on Proton, some blocked",
      L"Je nach Spiel; teils auf Proton aktiv, teils gesperrt",
      L"Selon le jeu ; certains actifs sur Proton, d'autres bloqués",
      L"Según el juego; algunos activos en Proton, otros bloqueados",
      L"Dipende dal gioco; alcuni attivi su Proton, altri bloccati",
      L"Por jogo; alguns ativos no Proton, outros bloqueados",
      L"Зависит от игры; часть работает в Proton, часть заблокирована",
      L"ゲームによる。Proton で有効なものと無効なものがある",
      L"因游戏而异；部分在 Proton 上启用，部分被阻止",
      L"게임마다 다름; 일부는 Proton에서 활성, 일부는 차단",
      L"Per game; sommige actief op Proton, andere geblokkeerd",
      L"Zależnie od gry; część aktywna na Proton, część zablokowana",
      L"Залежить від гри; частина активна в Proton, частина заблокована" }} },
  { L"Popsicle / balenaEtcher / Disk Yazıcı", {{
      L"Popsicle / balenaEtcher / Disk Yazıcı",
      L"Popsicle / balenaEtcher / Disk Writer",
      L"Popsicle / balenaEtcher / Laufwerk-Schreiber",
      L"Popsicle / balenaEtcher / Graveur de disque",
      L"Popsicle / balenaEtcher / Grabador de discos",
      L"Popsicle / balenaEtcher / Scrittore disco",
      L"Popsicle / balenaEtcher / Gravador de disco",
      L"Popsicle / balenaEtcher / Запись на диск",
      L"Popsicle / balenaEtcher / ディスク書き込み",
      L"Popsicle / balenaEtcher / 磁盘写入器",
      L"Popsicle / balenaEtcher / 디스크 라이터",
      L"Popsicle / balenaEtcher / Schijfschrijver",
      L"Popsicle / balenaEtcher / Zapis dysku",
      L"Popsicle / balenaEtcher / Запис диска" }} },
  { L"Resmî Linux başlatıcısı mevcut", {{
      L"Resmî Linux başlatıcısı mevcut",
      L"Official Linux launcher available",
      L"Offizieller Linux-Launcher verfügbar",
      L"Lanceur Linux officiel disponible",
      L"Lanzador oficial de Linux disponible",
      L"Launcher Linux ufficiale disponibile",
      L"Iniciador oficial para Linux disponível",
      L"Есть официальный лаунчер для Linux",
      L"公式の Linux ランチャーがあります",
      L"提供官方 Linux 启动器",
      L"공식 Linux 런처 제공",
      L"Officiële Linux-launcher beschikbaar",
      L"Dostępny oficjalny launcher dla Linuksa",
      L"Доступний офіційний лаунчер для Linux" }} },
  { L"Resmî Linux desteği sınırlı; Linux Hyprland/KDE alternatifleri var", {{
      L"Resmî Linux desteği sınırlı; Linux Hyprland/KDE alternatifleri var",
      L"Limited official Linux support; Hyprland/KDE alternatives exist",
      L"Eingeschränkter offizieller Linux-Support; Hyprland/KDE-Alternativen",
      L"Support Linux officiel limité ; alternatives Hyprland/KDE",
      L"Soporte oficial de Linux limitado; alternativas Hyprland/KDE",
      L"Supporto Linux ufficiale limitato; alternative Hyprland/KDE",
      L"Suporte oficial ao Linux limitado; alternativas Hyprland/KDE",
      L"Ограниченная официальная поддержка Linux; есть альтернативы Hyprland/KDE",
      L"公式 Linux サポートは限定的。Hyprland/KDE の代替あり",
      L"官方 Linux 支持有限；有 Hyprland/KDE 替代",
      L"공식 Linux 지원 제한적; Hyprland/KDE 대안 있음",
      L"Beperkte officiële Linux-ondersteuning; Hyprland/KDE-alternatieven",
      L"Ograniczone oficjalne wsparcie Linuksa; alternatywy Hyprland/KDE",
      L"Обмежена офіційна підтримка Linux; є альтернативи Hyprland/KDE" }} },
  { L"Teams (web/PWA) / yerel istemci sınırlı", {{
      L"Teams (web/PWA) / yerel istemci sınırlı",
      L"Teams (web/PWA) / native client is limited",
      L"Teams (Web/PWA) / nativer Client eingeschränkt",
      L"Teams (web/PWA) / client natif limité",
      L"Teams (web/PWA) / cliente nativo limitado",
      L"Teams (web/PWA) / client nativo limitato",
      L"Teams (web/PWA) / cliente nativo limitado",
      L"Teams (веб/PWA) / нативный клиент ограничен",
      L"Teams（web/PWA）/ ネイティブクライアントは限定的",
      L"Teams（网页/PWA）/ 原生客户端有限",
      L"Teams(웹/PWA) / 네이티브 클라이언트 제한적",
      L"Teams (web/PWA) / native client is beperkt",
      L"Teams (web/PWA) / natywny klient ograniczony",
      L"Teams (веб/PWA) / нативний клієнт обмежений" }} },
  { L"Vanguard anti-cheat Linux'ta ÇALIŞMAZ (Valorant oynanamaz)", {{
      L"Vanguard anti-cheat Linux'ta ÇALIŞMAZ (Valorant oynanamaz)",
      L"Vanguard anti-cheat does NOT work on Linux (Valorant unplayable)",
      L"Vanguard-Anti-Cheat funktioniert NICHT unter Linux (Valorant nicht spielbar)",
      L"L'anti-triche Vanguard ne fonctionne PAS sous Linux (Valorant injouable)",
      L"El anti-trampas Vanguard NO funciona en Linux (Valorant no jugable)",
      L"L'anti-cheat Vanguard NON funziona su Linux (Valorant ingiocabile)",
      L"O anti-trapaça Vanguard NÃO funciona no Linux (Valorant injogável)",
      L"Античит Vanguard НЕ работает на Linux (Valorant не запустить)",
      L"Vanguard アンチチートは Linux で動作しません（Valorant プレイ不可）",
      L"Vanguard 反作弊在 Linux 上无法运行（无法游玩 Valorant）",
      L"Vanguard 안티치트는 Linux에서 작동 안 함(발로란트 불가)",
      L"Vanguard-anti-cheat werkt NIET op Linux (Valorant onspeelbaar)",
      L"Anti-cheat Vanguard NIE działa na Linuksie (Valorant niegrywalny)",
      L"Античит Vanguard НЕ працює на Linux (Valorant не грати)" }} },
  { L"Vanguard anti-cheat nedeniyle Linux'ta çalışmaz", {{
      L"Vanguard anti-cheat nedeniyle Linux'ta çalışmaz",
      L"Won't run on Linux due to Vanguard anti-cheat",
      L"Läuft wegen Vanguard-Anti-Cheat nicht unter Linux",
      L"Ne fonctionne pas sous Linux (anti-triche Vanguard)",
      L"No funciona en Linux por el anti-trampas Vanguard",
      L"Non funziona su Linux per l'anti-cheat Vanguard",
      L"Não funciona no Linux devido ao anti-trapaça Vanguard",
      L"Не работает на Linux из-за античита Vanguard",
      L"Vanguard アンチチートのため Linux で動作しません",
      L"因 Vanguard 反作弊，无法在 Linux 上运行",
      L"Vanguard 안티치트로 Linux에서 실행 안 됨",
      L"Werkt niet op Linux door Vanguard-anti-cheat",
      L"Nie działa na Linuksie z powodu anti-cheat Vanguard",
      L"Не працює на Linux через античит Vanguard" }} },
  { L"Vanguard sonrası Linux'ta çalışmaz", {{
      L"Vanguard sonrası Linux'ta çalışmaz",
      L"Won't run on Linux since Vanguard",
      L"Läuft seit Vanguard nicht unter Linux",
      L"Ne fonctionne plus sous Linux depuis Vanguard",
      L"No funciona en Linux desde Vanguard",
      L"Non funziona su Linux dopo Vanguard",
      L"Não funciona no Linux desde o Vanguard",
      L"Не работает на Linux после Vanguard",
      L"Vanguard 導入後 Linux で動作しません",
      L"自 Vanguard 起无法在 Linux 上运行",
      L"Vanguard 이후 Linux에서 실행 안 됨",
      L"Werkt niet op Linux sinds Vanguard",
      L"Nie działa na Linuksie od czasu Vanguard",
      L"Не працює на Linux після Vanguard" }} },
  { L"Wine/Bottles ile çalışabilir; alternatif: LMMS / Reaper", {{
      L"Wine/Bottles ile çalışabilir; alternatif: LMMS / Reaper",
      L"May work via Wine/Bottles; alternative: LMMS / Reaper",
      L"Läuft evtl. über Wine/Bottles; Alternative: LMMS / Reaper",
      L"Peut fonctionner via Wine/Bottles ; alternative : LMMS / Reaper",
      L"Puede funcionar con Wine/Bottles; alternativa: LMMS / Reaper",
      L"Può funzionare con Wine/Bottles; alternativa: LMMS / Reaper",
      L"Pode funcionar via Wine/Bottles; alternativa: LMMS / Reaper",
      L"Может работать через Wine/Bottles; альтернатива: LMMS / Reaper",
      L"Wine/Bottles で動作する場合あり。代替: LMMS / Reaper",
      L"可通过 Wine/Bottles 运行；替代：LMMS / Reaper",
      L"Wine/Bottles로 작동 가능; 대안: LMMS / Reaper",
      L"Kan werken via Wine/Bottles; alternatief: LMMS / Reaper",
      L"Może działać przez Wine/Bottles; alternatywa: LMMS / Reaper",
      L"Може працювати через Wine/Bottles; альтернатива: LMMS / Reaper" }} },
  { L"Xbox uygulaması yok; Cloud Gaming tarayıcıdan çalışır", {{
      L"Xbox uygulaması yok; Cloud Gaming tarayıcıdan çalışır",
      L"No Xbox app; Cloud Gaming works in the browser",
      L"Keine Xbox-App; Cloud Gaming läuft im Browser",
      L"Pas d'app Xbox ; le Cloud Gaming fonctionne dans le navigateur",
      L"Sin app de Xbox; Cloud Gaming funciona en el navegador",
      L"Nessuna app Xbox; Cloud Gaming funziona nel browser",
      L"Sem app Xbox; Cloud Gaming funciona no navegador",
      L"Нет приложения Xbox; Cloud Gaming работает в браузере",
      L"Xbox アプリなし。Cloud Gaming はブラウザで動作",
      L"无 Xbox 应用；Cloud Gaming 可在浏览器中运行",
      L"Xbox 앱 없음; Cloud Gaming은 브라우저에서 작동",
      L"Geen Xbox-app; Cloud Gaming werkt in de browser",
      L"Brak aplikacji Xbox; Cloud Gaming działa w przeglądarce",
      L"Немає застосунку Xbox; Cloud Gaming працює у браузері" }} },
  { L"Xed (yerleşik) / Kate — Wine ile NP++ da çalışır", {{
      L"Xed (yerleşik) / Kate — Wine ile NP++ da çalışır",
      L"Xed (built-in) / Kate — NP++ also runs via Wine",
      L"Xed (integriert) / Kate — NP++ läuft auch über Wine",
      L"Xed (intégré) / Kate — NP++ fonctionne aussi via Wine",
      L"Xed (integrado) / Kate — NP++ también funciona con Wine",
      L"Xed (integrato) / Kate — NP++ funziona anche con Wine",
      L"Xed (integrado) / Kate — NP++ também funciona via Wine",
      L"Xed (встроенный) / Kate — NP++ тоже работает через Wine",
      L"Xed（内蔵）/ Kate — NP++ も Wine で動作",
      L"Xed（内置）/ Kate — NP++ 也可通过 Wine 运行",
      L"Xed(기본 제공) / Kate — NP++도 Wine으로 작동",
      L"Xed (ingebouwd) / Kate — NP++ werkt ook via Wine",
      L"Xed (wbudowany) / Kate — NP++ też działa przez Wine",
      L"Xed (вбудований) / Kate — NP++ теж працює через Wine" }} },
  { L"Yerel Linux sürümü mevcut (emülatör)", {{
      L"Yerel Linux sürümü mevcut (emülatör)",
      L"Native Linux version available (emulator)",
      L"Native Linux-Version verfügbar (Emulator)",
      L"Version Linux native disponible (émulateur)",
      L"Versión nativa de Linux disponible (emulador)",
      L"Versione Linux nativa disponibile (emulatore)",
      L"Versão nativa para Linux disponível (emulador)",
      L"Есть нативная версия для Linux (эмулятор)",
      L"ネイティブの Linux 版があります（エミュレーター）",
      L"提供原生 Linux 版本（模拟器）",
      L"네이티브 Linux 버전 제공(에뮬레이터)",
      L"Native Linux-versie beschikbaar (emulator)",
      L"Dostępna natywna wersja dla Linuksa (emulator)",
      L"Доступна нативна версія для Linux (емулятор)" }} },
  { L"Yerel Linux sürümü mevcut (ücretsiz)", {{
      L"Yerel Linux sürümü mevcut (ücretsiz)",
      L"Native Linux version available (free)",
      L"Native Linux-Version verfügbar (kostenlos)",
      L"Version Linux native disponible (gratuite)",
      L"Versión nativa de Linux disponible (gratis)",
      L"Versione Linux nativa disponibile (gratuita)",
      L"Versão nativa para Linux disponível (gratuita)",
      L"Есть нативная версия для Linux (бесплатно)",
      L"ネイティブの Linux 版があります（無料）",
      L"提供原生 Linux 版本（免费）",
      L"네이티브 Linux 버전 제공(무료)",
      L"Native Linux-versie beschikbaar (gratis)",
      L"Dostępna natywna wersja dla Linuksa (bezpłatna)",
      L"Доступна нативна версія для Linux (безкоштовно)" }} },
  { L"Yerleşik terminal (ssh) / GNOME Terminal", {{
      L"Yerleşik terminal (ssh) / GNOME Terminal",
      L"Built-in terminal (ssh) / GNOME Terminal",
      L"Integriertes Terminal (ssh) / GNOME-Terminal",
      L"Terminal intégré (ssh) / GNOME Terminal",
      L"Terminal integrado (ssh) / GNOME Terminal",
      L"Terminale integrato (ssh) / GNOME Terminal",
      L"Terminal integrado (ssh) / GNOME Terminal",
      L"Встроенный терминал (ssh) / GNOME Terminal",
      L"内蔵ターミナル（ssh）/ GNOME 端末",
      L"内置终端 (ssh) / GNOME 终端",
      L"기본 터미널(ssh) / GNOME 터미널",
      L"Ingebouwde terminal (ssh) / GNOME Terminal",
      L"Wbudowany terminal (ssh) / GNOME Terminal",
      L"Вбудований термінал (ssh) / GNOME Terminal" }} },
};

// Translates a DB note to the selected language; returns it as-is if no pattern matches.
inline std::wstring trNote(const std::wstring& note) {
    for (const auto& n : NOTE_MAP)
        if (note == n.tr) return (const wchar_t*)n.tx;
    return note;   // product name / original note — don't translate
}

} // namespace str

// Txt formatter for std::format — so call sites stay unchanged
template<>
struct std::formatter<str::Txt, wchar_t> : std::formatter<std::wstring_view, wchar_t> {
    template<class Ctx>
    auto format(const str::Txt& t, Ctx& ctx) const {
        return std::formatter<std::wstring_view, wchar_t>::format(
            std::wstring_view((const wchar_t*)t), ctx);
    }
};

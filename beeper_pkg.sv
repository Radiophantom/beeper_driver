/*
  CSR для доступа к beeper'у

  Настройка частоты бипера:
    В бипере есть генератор частоты, построенный на NCO.
    По умолчанию он стартует с частотой 1000 Гц.
    Эту частоту можно изменить.

    Процесс настройки:
      1. Вычисляем новую "фазу" генератора частоты.
      2. Записываем в BEEPER_OSC_PHASE_W1_CR/W0_CR.
      3. Дергаем строб BEEPER_MAIN_CR_OSC_PHASE_WR_STB.

    Вычисление фазы происходит по следующей формуле:
      phase = 2^32 / CLK_FREQ * freq,
      где:
        CLK_FREQ - значение частоты, на которой работает модуль бипера. ( в Гц ).
                   в нашем случае это 62_500_000.
        freq     - требуемая частота ( в Гц ).

  Режимы работы бипера:
    OFF        = 0x0; // выключен
    ON         = 0x1; // постоянно включен
    TIME       = 0x2; // включена генерации в течении определенного времени
    FORCE_ZERO = 0x3; // подать на выходной пин 0
    FORCE_ONE  = 0x4; // подать на выходной пин 1

  Просто записывается в BEEPER_MAIN_CR_MODE_B2/B0.

  Само бипание:
    Под бипанием понимаем отсылку меандра с частотой, на которую настроен генератор.

    "Ручное":
      1. Ставим режим работы ON.
      2. Ждем нужное время (через sleep, NutSleep).
      3. Ставим режим работы OFF.

    "С таймером":
      1. Ставим режим работы TIME.
      2. Записываем время генерации в тиках в BEEPER_GEN_TIME_W1_CR/W0_CR.
      3. Дергаем строб BEEPER_MAIN_CR_OSC_PHASE_WR_STB.
      4. После этого строба в течении BEEPER_GEN_TIME будет происходить пикание.

    Так же можно вручную выставлять уровень сигнала на пине используя режимы FORCE_ZERO и FORCE_ONE.
*/

package beeper_pkg;

parameter BEEPER_VER = 16'h0_0_0_1;

/* CONTROL REGISTERS */

parameter BEEPER_MAIN_CR = 0;
        // выбор режима работа бипера
        parameter BEEPER_MAIN_CR_MODE_B0 = 0;
        parameter BEEPER_MAIN_CR_MODE_B2 = 2;

        // строб для старта таймера
        parameter BEEPER_MAIN_CR_TIMER_RUN_STB = 14;

        // строб для записи новой "фазы" генератора частоты
        parameter BEEPER_MAIN_CR_OSC_PHASE_WR_STB = 15;

// время генерации, в тактах
parameter BEEPER_GEN_TIME_W0_CR = 1;
parameter BEEPER_GEN_TIME_W1_CR = 2;

// фаза генератора.
parameter BEEPER_OSC_PHASE_W0_CR = 3;
parameter BEEPER_OSC_PHASE_W1_CR = 4;

        parameter BEEPER_CR_CNT = 5;

/* STATUS REGISTERS */

parameter BEEPER_VER_SR = 0;

parameter BEEPER_SR_CNT = 1;

endpackage

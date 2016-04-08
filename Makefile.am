bin_PROGRAMS = development development_cluster
development_SOURCES = sample/development.c
development_LDADD = libcresus.la
development_cluster_SOURCES = sample/development_cluster.c
development_cluster_LDADD = libcresus.la
lib_LTLIBRARIES = libcresus.la
libcresus_la_SOURCES = framework/indicator.c framework/timeline_entry.c \
		       engine/candle.c engine/timeline.c engine/cluster.c \
                       math/average.c input/yahoo.c \
		       indicator/atr.c indicator/bollinger.c \
                       indicator/heikin_ashi.c indicator/rs_dorsey.c \
                       indicator/macd.c indicator/mobile.c indicator/rsi.c \
                       indicator/smi.c indicator/srsi.c \
                       indicator/stochastic.c indicator/stoploss.c \
		       indicator/rs_mansfield.c \
                       indicator/swing.c indicator/zigzag.c
#		       input/google.c input/yahoo.c \
                       framework/module.c engine/engine.c \
                       trading/portfolio.c trading/position.c \
                       trading/statistics.c trading/trade.c

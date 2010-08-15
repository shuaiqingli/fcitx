tabledir = $(pkgdatadir)/data/table

mb_data = cj.mb erbi.mb wanfeng.mb wbpy.mb qxm.mb db.mb wbx.mb zrm.mb
mb_conf = bingcan.conf db.conf erbi.conf wanfeng.conf  wbpy.conf wbx.conf zrm.conf cangjie.conf

table_DATA = $(mb_data) $(mb_conf)
EXTRA_DIST = $(table_DATA)

%.mb: %.txt
	../../tools/txt2mb $< $@

tools: ../../tools/txt2mb
	$(MAKE) -C ../../tools

all: tools $(mb_data)

clean:
	rm -f $(mb_data)

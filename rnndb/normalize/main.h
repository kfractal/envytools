// -*- mode: c++; tab-width: 4; indent-tabs-mode: t; -*-
#include <QCoreApplication>
#include <QFile>
#include <QDir>

#if 0
class RootDoc : public QObject
{
	Q_OBJECT
public:
	RootDoc(QFile *);
public slots:
	void read_doc();
signals:
	void follow_doc(QFile *);
protected:
};
#endif

class Main : public QCoreApplication
{
	Q_OBJECT
public:
	Main(int & argc, char ** argv);
public slots:
	void start();
protected:
	static constexpr bool _debug   = false;
	static constexpr bool _verbose = false;
	static constexpr bool _warn    = false;
	bool _error;
	QString _root;
	QDir _root_dir;

	int read_root();
	int cd_and_read(const QString &);
	int read_file(QFile &);
};

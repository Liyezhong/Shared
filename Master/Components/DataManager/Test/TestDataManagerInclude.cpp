#include <QTest>
#include <QObject>
#include <QDebug>

namespace DataManager {
  class TestDataManagerInclude : public QObject {
      Q_OBJECT
  public:
      TestDataManagerInclude()
      {

      }

  public:

  private slots:
      void initTestCase()
      {
          qDebug() << __FUNCTION__;
      }

      void cleanupTestCase()
      {
          qDebug() << __FUNCTION__;
      }
  };

} // end namespace

QTEST_MAIN(DataManager::TestDataManagerInclude)

#include "TestDataManagerInclude.moc"




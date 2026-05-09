// spriteatlas.h
#pragma once

#include <QMap>
#include <QString>
#include <QPixmap>

class BehaviourData;   // forward

class SpriteAtlas {
public:
    void load(const BehaviourData &data, const QString &spritesFolder);
    const QPixmap* get(const QString &name) const;
    QSize frameSize() const;

private:
    QMap<QString, QPixmap> m_pixmaps;
    QSize m_size;
};

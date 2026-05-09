#include "SpriteAtlas.h"
#include "BehaviourData.h"
#include <QDir>

void SpriteAtlas::load(const BehaviourData &data, const QString &spritesFolder) {
    QDir dir(spritesFolder);
    for (auto it = data.animations.begin(); it != data.animations.end(); ++it) {
        const AnimationDef &def = it.value();
        for (int i = 1; i <= def.frameCount; ++i) {
            QString fileName = def.prefix + QString::number(i) + ".png";
            QPixmap pix(dir.filePath(fileName));
            if (!pix.isNull()) {
                pix = pix.scaled(pix.size() * data.scaleFactor,
                                 Qt::IgnoreAspectRatio,
                                 Qt::FastTransformation);
                m_pixmaps.insert(fileName, pix);
                m_size = pix.size();
            }
        }
    }
}

const QPixmap* SpriteAtlas::get(const QString &name) const {
    auto it = m_pixmaps.find(name);
    return it != m_pixmaps.cend() ? &it.value() : nullptr;
}

QSize SpriteAtlas::frameSize() const { return m_size; }

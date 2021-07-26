#pragma once

#include <QtWidgets/QAbstractScrollArea>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLineEdit>

#include "dockhandler.h"
#include "render.h"
#include "uitypes.h"

class BINARYNINJAUIAPI CreateStackVariableDialog : public QDialog {
    Q_OBJECT

    BinaryViewRef m_data;
    FunctionRef m_func;

    QLineEdit* m_offsetField;
    QLineEdit* m_nameField;
    QComboBox* m_typeDropdown;

    void autoFillFields();

protected:
    void accept();

public:
    CreateStackVariableDialog(QWidget* parent, BinaryViewRef data,
        FunctionRef func, int64_t initialOffset = 0);
};

class StackViewLine {
public:
    enum class Type {
        Variable,
        Member,
        OffsetRef,
        Annotation,
        Fill
    };

    static StackViewLine variable(int64_t offset,
        BinaryNinja::VariableNameAndType vnat, PlatformRef plat);
    static StackViewLine member(int64_t offset,
        BinaryNinja::VariableNameAndType vnat, PlatformRef plat);
    static StackViewLine offsetRef(int64_t base, uint64_t offset, size_t size);
    static StackViewLine annotation(int64_t offset, const std::string& text);
    static StackViewLine fill(int64_t offset, size_t length);

    StackViewLine::Type type() const;

    int64_t offset() const;
    size_t width() const;

    int64_t relativeOffset() const;
    void setRelativeOffset(int64_t relativeOffset);

    size_t level() const;
    void setLevel(size_t level);

    void setDataType(TypeRef vnat);
    void setWidthOverride(size_t width);
    bool isDataBacked() const;

    BinaryNinja::DisassemblyTextLine content() const;
    BinaryNinja::DisassemblyTextLine contentWithOffset(bool hide = false) const;

    void appendToken(BinaryNinja::InstructionTextToken token);
    void appendTokens(std::vector<BinaryNinja::InstructionTextToken> tokens);
    void indent(size_t levels = 1);

private:
    StackViewLine(StackViewLine::Type type, int64_t offset);

    StackViewLine::Type m_type;
    int64_t m_offset;
    int64_t m_relativeOffset;
    size_t m_level;

    TypeRef m_dataType;
    size_t m_widthOverride;

    BinaryNinja::DisassemblyTextLine m_content;
};

class BINARYNINJAUIAPI StackView : public QAbstractScrollArea,
                                   public View,
                                   public DockContextHandler {
    Q_OBJECT
    Q_INTERFACES(DockContextHandler)

    ViewFrame* m_view;
    BinaryViewRef m_data;
    FunctionRef m_func;
    RenderContext m_renderer;

    std::vector<StackViewLine> m_lines;
    HighlightTokenState m_highlight;
    size_t m_cursorLine = 0;
    size_t m_cursorIndex = 0;

    void setupActions();

    void rebuildLines();

protected:
    void paintEvent(QPaintEvent* event);
    void mousePressEvent(QMouseEvent* event);

public:
    StackView(ViewFrame* view, BinaryViewRef data);

    //! Refresh the stack view's content.
    void refresh();
    void moveCursorToMouse(QMouseEvent* event, bool isSelecting);

    void rename();
    void retype();
    void showCreateVariableDialog();
    void quickCreateVariableAtCursor(size_t size);

    // --- View Interface ---
    BinaryViewRef getData();
    uint64_t getCurrentOffset();
    void setSelectionOffsets(BNAddressRange range);
    bool navigate(uint64_t offset);
    QFont getFont();
};

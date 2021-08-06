#pragma once

#include <QtWidgets/QAbstractScrollArea>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLineEdit>

#include "dockhandler.h"
#include "render.h"
#include "sidebar.h"
#include "uitypes.h"

enum class Direction {
    Up,
    Down,
    Left,
    Right
};

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

    TypeRef dataType() const;
    void setDataType(TypeRef vnat);

    BinaryNinja::Variable variable() const;
    void setVariable(BinaryNinja::Variable var);

    void setWidthOverride(size_t width);
    bool isDataBacked() const;

    bool isReferenced() const;
    void setIsReferenced(bool isReferenced);

    bool isUnused() const;
    void setIsUnused(bool isUnused);

    BinaryNinja::DisassemblyTextLine content() const;
    BinaryNinja::DisassemblyTextLine contentWithOffset(bool hide = false) const;
    size_t contentWidth(bool withOffset = true) const;

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
    BinaryNinja::Variable m_var;
    size_t m_widthOverride;

    bool m_isReferenced;
    bool m_isUnused;

    BinaryNinja::DisassemblyTextLine m_content;
};

class BINARYNINJAUIAPI StackView : public QAbstractScrollArea,
                                   public View {
    Q_OBJECT

    ViewFrame* m_view;
    BinaryViewRef m_data;
    FunctionRef m_func;
    RenderContext m_renderer;

    std::vector<StackViewLine> m_lines;
    HighlightTokenState m_highlight;
    size_t m_lineIndex;
    size_t m_tokenIndex;

    void setupActions();
    void rebuildLines();

    int64_t findVoidEnd(int64_t start) const;

protected:
    void paintEvent(QPaintEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseDoubleClickEvent(QMouseEvent* event);

public:
    StackView(ViewFrame* view, BinaryViewRef data);

    //! Refresh the stack view's content.
    void refresh();
    void moveCursorToMouse(QMouseEvent* event, bool isSelecting);
    void moveCursor(Direction dir);
    void sanitizeCursor(Direction preference);
    void refreshHighlight();

    StackViewLine selectedLine() const;

    void renameVariable();
    void retypeVariable();
    void undefineVariable();

    void showCreateVariableDialog();

    void quickCreateVariableAtCursor(size_t size);
    void quickCreateArrayAtCursor();
    void quickCreateStructAtCursor();

    // --- View Interface ---
    BinaryViewRef getData();
    uint64_t getCurrentOffset();
    void setSelectionOffsets(BNAddressRange range);
    bool navigate(uint64_t offset);
    QFont getFont();
    void updateFonts() override;
};

class BINARYNINJAUIAPI StackViewSidebarWidget : public SidebarWidget {
    Q_OBJECT

    StackView* m_sv;
    QWidget* m_header;

public:
    StackViewSidebarWidget(ViewFrame* view, BinaryViewRef data);

    void refresh();

    void focus() override { refresh(); }
    void notifyFontChanged() override { m_sv->updateFonts(); }

    QWidget* headerWidget() override { return m_header; }
};

class BINARYNINJAUIAPI StackViewSidebarWidgetType : public SidebarWidgetType {
public:
    StackViewSidebarWidgetType();
    SidebarWidget* createWidget(ViewFrame* frame, BinaryViewRef data) override;
};

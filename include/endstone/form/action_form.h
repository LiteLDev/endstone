// Copyright (c) 2024, The Endstone Project. (https://endstone.dev) All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <optional>
#include <string>

#include "endstone/form/form.h"

namespace endstone {

/**
 * @brief Represents a form with buttons that let the player take action.
 */
class ActionForm : public Form<ActionForm> {
public:
    /**
     * @brief Represents a button with text and an optional icon.
     */
    class Button {
    public:
        Button() = default;
        explicit Button(Message text, std::optional<std::string> icon = std::nullopt)
            : text_(std::move(text)), icon_(std::move(icon))
        {
        }

        /**
         * @brief Gets the text of the button.
         *
         * @return The text.
         */
        [[nodiscard]] Message getText() const
        {
            return text_;
        }

        /**
         * @brief Sets the text of the button.
         *
         * @param text The new text for the button.
         * @return A reference to the current button.
         */
        Button &setText(Message text)
        {
            text_ = std::move(text);
            return *this;
        }

        /**
         * @brief Get the icon of the button.
         *
         * @return The path or URL to the icon file
         */
        [[nodiscard]] std::optional<std::string> getIcon() const
        {
            return icon_;
        }

        /**
         * @brief Sets the icon for the button.
         *
         * @param icon The path or URL to the icon file.
         * @return A reference to the current button.
         */
        Button &setIcon(std::string icon)
        {
            icon_ = std::move(icon);
            return *this;
        }

    private:
        Message text_;
        std::optional<std::string> icon_;
    };

    /**
     * @brief Get the content of the form.
     *
     * @return The content of the form.
     */
    [[nodiscard]] Message getContent() const
    {
        return content_;
    }

    /**
     * @brief Set the content of the form.
     *
     * @param text The text to set as the content.
     * @return A reference to the current form.
     */
    ActionForm &setContent(Message text)
    {
        content_ = std::move(text);
        return *this;
    }

    /**
     * @brief Adds a button to the form.
     *
     * @param text The text of the button
     * @param icon The path or URL to the icon image
     * @return A reference to the current form.
     */
    ActionForm &addButton(const Message &text, const std::optional<std::string> &icon = std::nullopt)
    {
        buttons_.emplace_back(text, icon);
        return *this;
    }

    /**
     * @brief Get the buttons of the action form.
     *
     * @return A list of buttons in the action form.
     */
    [[nodiscard]] const std::vector<Button> &getButtons() const
    {
        return buttons_;
    }

    /**
     * @brief Set the buttons of the action form.
     *
     * @param buttons The list of buttons to set.
     * @return A reference to the current form.
     */
    ActionForm &setButtons(const std::vector<Button> &buttons)
    {
        buttons_ = buttons;
        return *this;
    }

private:
    Message content_;
    std::vector<Button> buttons_;
};

}  // namespace endstone

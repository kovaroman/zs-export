<?hh // strict
namespace ElasticExport\Helper;

use Plenty\Modules\Category\Contracts\CategoryBranchRepositoryContract;
use Plenty\Modules\Item\DataLayer\Models\Record;
use Plenty\Modules\Helper\Models\KeyValue;

/**
 * Class ElasticExportHelper
 * @package ElasticExportHelper\Helper
 */
class ElasticExportHelper
{
    const string SHIPPING_COST_TYPE_FLAT = 'flat';
    const string SHIPPING_COST_TYPE_CONFIGURATION = 'configuration';

    const string IMAGE_POSITION0 = 'position0';
    const string IMAGE_FIRST = 'firstImage';

    const int REMOVE_HTML_TAGS = 1;
    const int KEEP_HTML_TAGS = 0;

    const int ITEM_URL_NO = 0;
    const int ITEM_URL_YES = 1;

    const int TRANSFER_ITEM_AVAILABILITY_NO = 0;
    const int TRANSFER_ITEM_AVAILABILITY_YES = 1;

    const int TRANSFER_OFFER_PRICE_NO = 0;
    const int TRANSFER_OFFER_PRICE_YES = 1;

    /**
     * CategoryBranchRepositoryContract $categoryBranchRepository
     */
    private CategoryBranchRepositoryContract $categoryBranchRepository;

    public function __construct(CategoryBranchRepositoryContract $categoryBranchRepository)
    {
        $this->categoryBranchRepository = $categoryBranchRepository;
    }

    /**
     * Get name.
     *
     * @param  Record    $item
     * @param  KeyValue  $settings
     * @param  int $defaultNameLength
     * @return string
     */
    public function getName(Record $item, KeyValue $settings, int $defaultNameLength):string
	{
		switch($settings->get('nameId'))
		{
			case 3:
				$name = $item->itemDescription->name3;
                break;

			case 2:
				$name = $item->itemDescription->name2;
                break;

			case 1:
			default:
				$name = $item->itemDescription->name1;
                break;
		}

        $nameLength = $settings->get('nameMaxLength') ? $settings->get('nameMaxLength') : $defaultNameLength;

        return $name;
    }

    /**
     * Get preview text.
     *
     * @param  Record        $item
     * @param  KeyValue      $settings
     * @param  int           $defaultPreviewTextLength
     * @return string
     */
    public function getPreviewText(Record $item, KeyValue $settings, int $defaultPreviewTextLength):string
    {
        switch($settings->get('previewTextType'))
        {
            case 'itemShortDescription':
                $previewText = $item->itemDescription->shortDescription;
                break;

            case 'technicalData':
                $previewText = $item->itemDescription->technicalData;
                break;

            case 'itemDescriptionAndTechnicalData':
                $previewText = $item->itemDescription->description . ' ' . $item->itemDescription->technicalData;
                break;

            case 'itemDescription':
                $previewText = $item->itemDescription->description;
                break;

            case 'dontTransfer':
            default:
                $previewText = '';
                break;
        }

        $descriptionLength = $settings->get('previewTextMaxLength') ? $settings->get('previewTextMaxLength') : $defaultPreviewTextLength;

        if($settings->get('previewTextRemoveHtmlTags') == self::REMOVE_HTML_TAGS)
        {
            $previewText = strip_tags($previewText, str_replace([',', ' '], '', $settings->get('previewTextAllowHtmlTags')));
        }

        return substr($previewText, 0, $descriptionLength);
    }

    /**
     * Get description.
     *
     * @param  Record        $item
     * @param  KeyValue      $settings
     * @param  int           $defaultDescriptionLength
     * @return string
     */
    public function getDescription(Record $item, KeyValue $settings, int $defaultDescriptionLength):string
    {
        switch($settings->get('descriptionType'))
        {
            case 'itemShortDescription':
                $description = $item->itemDescription->shortDescription;
                break;

            case 'technicalData':
                $description = $item->itemDescription->technicalData;
                break;

            case 'itemDescriptionAndTechnicalData':
                $description = $item->itemDescription->description . ' ' . $item->itemDescription->technicalData;
                break;

            case 'itemDescription':
            default:
                $description = $item->itemDescription->description;
                break;
        }

        $descriptionLength = $settings->get('descriptionMaxLength') ? $settings->get('descriptionMaxLength') : $defaultDescriptionLength;

        if($settings->get('descriptionRemoveHtmlTags') == self::REMOVE_HTML_TAGS)
        {
            $description = strip_tags($description, str_replace([',', ' '], '', $settings->get('previewTextAllowHtmlTags')));
        }

        return substr($description, 0, $descriptionLength);
    }

    /**
	 * Get variation availability days.
	 * @param  Record   $item
	 * @param  KeyValue $settings
	 * @return int
	 */
	public function getAvailability(Record $item, KeyValue $settings):int
	{
		if($settings->get('transferItemAvailability') == self::TRANSFER_ITEM_AVAILABILITY_YES)
		{
            $availabilityIdString = 'itemAvailability' . $item->variationBase->availability;

		    return $settings->get($availabilityIdString);
		}

		// TODO match variation avialibility Id with the system configuration

		return 1;
	}

    /**
     * Get the item URL.
     * @param  {[type]} Record    $item
     * @param  {[type]} KeyValue  $settings
     * @param  {[type]} bool      $addReferrer    =             true  Choose if referrer id should be added as parameter.
     * @param  {[type]} bool      $useIntReferrer =             false Choos if referrer id should be used as integer.
     * @return {[type]}           Item url.
     */
    public function getUrl(Record $item, KeyValue $settings, bool $addReferrer = true, bool $useIntReferrer = false):string
	{
        if($settings->get('itemUrl') == self::ITEM_URL_NO)
        {
            return '';
        }

		$urlParams = [];

        $link = ''; // TODO UrlBuilder get url_content

		if($addReferrer && $settings->get('referrerId'))
		{
            $urlParams[] = 'ReferrerID=' . ($useIntReferrer ? (int) $settings->get('referrerId') : $settings->get('referrerId'));
		}

		if(strlen($settings->get('urlParam')))
		{
			$urlParams[] = $settings->get('urlParam');
		}

		if (is_array($urlParams) && count($urlParams) > 0)
		{
			$link .= '?' . implode('&', $urlParams);
		}

		return $link;
	}

    /**
     * Get category.
     * @param  Record   $item
     * @param  KeyValue $settings
     * @param  string   $separator
     * @return string
     */
    public function getCategory(Record $item, KeyValue $settings, string $separator = ' > '):string
	{
        return 'Category: ' . $item->variationStandardCategory->categoryId;

        $categoryBranch = $this->categoryBranchRepository->findCategoryBranch($item->variationStandardCategory->categoryId);

        if(is_array($categoryBranch->getBranchName()) && count($categoryBranch))
		{
			return implode($separator, $categoryBranch->getBranchName());
		}

		return '';
	}

    /**
     * Get shipping cost.
     * @param  Record   $item
     * @param  KeyValue $settings
     * @return float
     */
    public function getShippingCost(Record $item, KeyValue $settings):float
    {
        if($settings->get('shippingCostType') == self::SHIPPING_COST_TYPE_FLAT)
        {
            return $settings->get('shippingCostFlat');
        }

        switch($settings->get('shippingCostConfiguration'))
        {
            case 1:
                // TODO use new shipping cost helper

            case 2:
                // TODO use new shipping cost helper
        }


        return 1.2;
    }

    public function getPrice(Record $item, KeyValue $settings):float
    {
        if($settings->get('transferOfferPrice') == self::TRANSFER_OFFER_PRICE_YES)
        {
            return $item->variationRetailPrice->price;
        }

        return 0.0;
    }

    /**
     * Get payement extra charge.
     * @param  Record   $item
     * @param  KeyValue $settings
     * @param  int      $methodOfPaymentId
     * @return float
     */
    public function getPaymentExtraCharge(Record $item, KeyValue $settings, int $methodOfPaymentId):float
    {
        $methodOfPaymentList = array(); // TODO call helper to get all method og payments

        if(is_array($methodOfPaymentList) && count($methodOfPaymentList) > 0)
		{
            if(1==1) // TODO check if the given mathodOfPaymentId is valid $methodOfPaymentList[$methodOfPaymentId] instanceof PaymentMethodData)
            {
                if($methodOfPaymentList[$methodOfPaymentId]->getFeeForeignPercentageWebshop())
                {
                    return ((float) $methodOfPaymentList[$methodOfPaymentId]->getFeeForeignPercentageWebshop() / 100) * $this->getPrice($item, $settings);
                }
                else
                {
                    return (float) $methodOfPaymentList[$methodOfPaymentId]->getFeeForeignFlatRateWebshop();
                }
            }
		}

        return 0.0;
    }

    /**
     * Get the attribute value set short frontend name. Ex. blue, XL
     * @param  Record   $item
     * @param  KeyValue $settings
     * @return string
     */
    public function getAttributeValueSetShortFrontendName(Record $item, KeyValue $settings):string
    {
        $attributeValueSetShortFrontendName = ''; // TODO call ItemDataLAyerHelperAttribute to get the attributeValueSetShortFrontendName
        return 'rot, xs';
    }

    /**
     * Get base price.
     * @param  Record   $item
     * @param  KeyValue $settings
     * @param  string   $separator         =             '/'
     * @param  bool     $compact           =             false
     * @param  bool     $dotPrice          =             false
     * @param  string   $currency          =             ''
     * @return string
     */
    public function getBasePrice(Record $item, KeyValue $settings, string $separator = '/', bool $compact = false, bool $dotPrice = false, string $currency = ''):string
	{
        $currency = strlen($currency) ? $currency : 'EUR'; // TODO $currency = get default currency
		$price = (float) $item->variationRetailPrice->price;
        $lot = (int) $item->variationBase->content;
        $unit = 'KGM'; // TODO ItemDataLayerHelperUnit::getUnitById($item->variationBase->unitId, $settings->get('lang'));

		$basePriceDetails = $this->getBasePriceDetails($lot, $price, $unit);

		if((float) $basePriceDetails['price'] <= 0 || ((int) $basePriceDetails['lot'] <= 1 && $basePriceDetails['unit'] == 'C62'))
		{
			return '';
		}

		if ($dotPrice == true)
		{
			$basePriceDetails['price'] = number_format($basePriceDetails['price'], 2, '.', '');
		}
		else
		{
			$basePriceDetails['price'] = number_format($basePriceDetails['price'], 2, ',', '');
		}

		if ($compact == true)
		{
			// TODO return	'('.$price.$currency.$separator.$lot.$this->getMeasureUnitShortcut($unit, $this->config->getLang()).')';
		}
		else
		{
			// TODO return	$price.' '.$currency.$separator. $lot.' '.$this->getMeasureUnit($unit, $this->config->getLang());
		}


        return 'base price';
	}

    /**
     * Get main image.
     * @param  Record   $item
     * @param  KeyValue $settings
     * @return string
     */
    public function getMainImage(Record $item, KeyValue $settings):string
    {
        foreach($item->variationImageList as $image)
        {
            if($settings->get('imagePosition') == self::IMAGE_FIRST)
            {
                return $image->path;
            }
            elseif($settings->get('imagePosition')== self::IMAGE_POSITION0 && $image->position == 0)
            {
                return $image->path;
            }
        }

        return '';
    }

    /**
     * Get item character value by backend name.
     * @param  Record $item
     * @param  string $backendName
     * @return string
     */
    public function getItemCharacterByBackendName(Record $item, string $backendName):string
    {
        foreach($item->itemCharacterList as $itemCharacter)
        {
            $itemCharacterBackendName = ''; // TODO get itemCharacterBackendName use helper

            if($itemCharacterBackendName == $backendName)
            {
                return (string) $itemCharacter->characterValue;
            }
        }

        return '';
    }


    /**
     * Get base price details.
     * @param  int    $lot
     * @param  float  $price
     * @param  string $unit
     * @return Map
     */
    private function getBasePriceDetails(int $lot, float $price, string $unit):Map<string,mixed>
    {
        $lot = $lot == 0 ? 1 : $lot; // TODO  PlentyStringUtils::numberFormatLot($lot, true);
		$basePrice = 0;
		$basePriceLot = 1;
        $unit = strlen($unit) ? $unit : 'C62';
        $basePriceUnit = $unit;

		$factor = 1.0;

		if($unit == 'LTR' || $unit == 'KGM')
		{
			$basePriceLot = 1;
		}
		elseif($unit == 'GRM' || $unit == 'MLT')
		{
			if($lot <= 250)
			{
				$basePriceLot = 100;
			}
			else
			{
				$factor = 1000.0;
				$basePriceLot = 1;
				$basePriceUnit = $unit =='GRM' ? 'KGM' : 'LTR';
			}
		}
		else
		{
			$basePriceLot = 1;
		}

		$endLot = ($basePriceLot/$lot);

		return Map{'lot' => (int) $basePriceLot, 'price' => (float) $price * $factor * $endLot, 'unit' => (string) $basePriceUnit};
    }
}
